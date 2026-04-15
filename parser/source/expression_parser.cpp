#include "../expression_parser.hpp"

#include <format>
#include <stack>
#include <cassert>

#include "../defs/parser_defs.hpp"

ExpressionParser::ExpressionParser(TokenConsumer& consumer) : tokenConsumer{ consumer } {}

std::unique_ptr<ASTExpr> ExpressionParser::parseExpr(){
    std::stack<std::unique_ptr<ASTExpr>> values;
    std::stack<std::unique_ptr<ASTBinaryExpr>> operators;

    auto reduce {
        [&values, &operators]() -> void {
            assert(values.size() > 1);
            assert(!operators.empty());

            auto op{ std::move(operators.top()) };
            operators.pop();

            auto right{ std::move(values.top()) };
            values.pop();

            auto left{ std::move(values.top()) };
            values.pop();

            op->setOperandExprs(std::move(left), std::move(right));
            values.push(std::move(op));
        }
    };

    values.push(parsePrimaryExpr());
    while(tokenConsumer.getToken().gtype == GeneralTokenType::OPERATOR){
        auto currentOp{ parseOperator() };
        const auto& currentOpToken{ currentOp->getToken() };

        while(!operators.empty() && 
            getPrecedence(operators.top()->getToken().type) >= 
            getPrecedence(currentOpToken.type)
        ){ reduce(); }

        operators.push(std::move(currentOp));
        values.push(parsePrimaryExpr());
    }

    while(!operators.empty()){
        reduce();
    }

    return std::move(values.top());
}

std::unique_ptr<ASTExpr> ExpressionParser::parsePrimaryExpr(){
    const auto& token{ tokenConsumer.getToken() };
    switch(token.type){
        case TokenType::LITERAL:
            return parseLiteralExpr();

        case TokenType::ID:
            if(tokenConsumer.peek().type == TokenType::LPAREN){
                return parseFunctionCallExpr();
            }
            return parseIdExpr();

        case TokenType::LPAREN:{
            tokenConsumer.consume(TokenType::LPAREN);
            auto expr{ parseExpr() };
            tokenConsumer.consume(TokenType::RPAREN);
            return expr;
        }
        default:
            break;
    }

    throw std::runtime_error(
        std::format(
            "Line {}, Column {}: SYNTAX ERROR -> expected 'EXPRESSION', got '{} {}'",
            token.line, token.column, tokenTypeToString.at(token.type), token.value
        )
    );
}

std::unique_ptr<ASTFunctionCallExpr> ExpressionParser::parseFunctionCallExpr(){
    std::unique_ptr<ASTFunctionCallExpr> callExpr{ 
        std::make_unique<ASTFunctionCallExpr>(tokenConsumer.getToken()) 
    };
    tokenConsumer.consume(TokenType::ID);
    
    tokenConsumer.consume(TokenType::LPAREN);
    parseArguments(callExpr.get());
    tokenConsumer.consume(TokenType::RPAREN);
    
    return callExpr;
}

void ExpressionParser::parseArguments(ASTFunctionCallExpr* callExpr){
    if(tokenConsumer.getToken().type == TokenType::RPAREN){
        return;
    }

    callExpr->addArgument(parseExpr());
    while(tokenConsumer.getToken().type == TokenType::COMMA){
        tokenConsumer.consume(TokenType::COMMA);
        callExpr->addArgument(parseExpr());
    }
}

std::unique_ptr<ASTIdExpr> ExpressionParser::parseIdExpr(){
    const auto& token{ tokenConsumer.getToken() };
    tokenConsumer.consume(TokenType::ID);

    return std::make_unique<ASTIdExpr>(token);
}

std::unique_ptr<ASTLiteralExpr> ExpressionParser::parseLiteralExpr(){
    const auto& token{ tokenConsumer.getToken() };
    tokenConsumer.consume(TokenType::LITERAL);
    if(token.value.back() == 'u'){
        return std::make_unique<ASTLiteralExpr>(token, Type::UNSIGNED);
    }
    return std::make_unique<ASTLiteralExpr>(token, Type::INT);
}

std::unique_ptr<ASTBinaryExpr> ExpressionParser::parseOperator(){
    std::unique_ptr<ASTBinaryExpr> op{ 
        std::make_unique<ASTBinaryExpr>(Token{ tokenConsumer.getToken() })
    };
    if(tokenTypeToOperator.find(tokenConsumer.getToken().type) != tokenTypeToOperator.end()){
        op->setOperator(tokenTypeToOperator.at(tokenConsumer.getToken().type));
    }

    tokenConsumer.consume(GeneralTokenType::OPERATOR);

    return op;
}