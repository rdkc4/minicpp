#include "../expression_parser.hpp"

#include <format>
#include <stack>
#include <cassert>

#include "../defs/parser_defs.hpp"

syntax::ExpressionParser::ExpressionParser(TokenConsumer& consumer) 
    : tokenConsumer{ consumer } {}

std::unique_ptr<syntax::ast::ASTExpr> 
syntax::ExpressionParser::parseExpr(){
    std::stack<std::unique_ptr<syntax::ast::ASTExpr>> values;
    std::stack<std::unique_ptr<syntax::ast::ASTBinaryExpr>> operators;

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
    while(tokenConsumer.getToken().gtype == syntax::GeneralTokenType::OPERATOR){
        auto currentOp{ parseOperator() };
        const auto& currentOpToken{ currentOp->getToken() };

        while(!operators.empty() && 
            syntax::getPrecedence(operators.top()->getToken().type) >= 
            syntax::getPrecedence(currentOpToken.type)
        ){ reduce(); }

        operators.push(std::move(currentOp));
        values.push(parsePrimaryExpr());
    }

    while(!operators.empty()){
        reduce();
    }

    return std::move(values.top());
}

std::unique_ptr<syntax::ast::ASTExpr> 
syntax::ExpressionParser::parsePrimaryExpr(){
    const auto& token{ tokenConsumer.getToken() };
    switch(token.type){
        case syntax::TokenType::LITERAL:
            return parseLiteralExpr();

        case syntax::TokenType::ID:
            if(tokenConsumer.peek().type == syntax::TokenType::LPAREN){
                return parseFunctionCallExpr();
            }
            return parseIdExpr();

        case syntax::TokenType::LPAREN:{
            tokenConsumer.consume(syntax::TokenType::LPAREN);
            auto expr{ parseExpr() };
            tokenConsumer.consume(syntax::TokenType::RPAREN);
            return expr;
        }
        default:
            break;
    }

    throw std::runtime_error(
        std::format(
            "Line {}, Column {}: SYNTAX ERROR -> expected 'EXPRESSION', got '{} {}'",
            token.line, token.column, syntax::tokenTypeToStr(token.type), token.value
        )
    );
}

std::unique_ptr<syntax::ast::ASTFunctionCallExpr> 
syntax::ExpressionParser::parseFunctionCallExpr(){
    std::unique_ptr<syntax::ast::ASTFunctionCallExpr> callExpr{ 
        std::make_unique<syntax::ast::ASTFunctionCallExpr>(tokenConsumer.getToken()) 
    };
    tokenConsumer.consume(syntax::TokenType::ID);
    
    tokenConsumer.consume(syntax::TokenType::LPAREN);
    parseArguments(callExpr.get());
    tokenConsumer.consume(syntax::TokenType::RPAREN);
    
    return callExpr;
}

void syntax::ExpressionParser::parseArguments(syntax::ast::ASTFunctionCallExpr* callExpr){
    if(tokenConsumer.getToken().type == syntax::TokenType::RPAREN){
        return;
    }

    callExpr->addArgument(parseExpr());
    while(tokenConsumer.getToken().type == syntax::TokenType::COMMA){
        tokenConsumer.consume(syntax::TokenType::COMMA);
        callExpr->addArgument(parseExpr());
    }
}

std::unique_ptr<syntax::ast::ASTIdExpr> 
syntax::ExpressionParser::parseIdExpr(){
    const auto& token{ tokenConsumer.getToken() };
    tokenConsumer.consume(syntax::TokenType::ID);

    return std::make_unique<syntax::ast::ASTIdExpr>(token);
}

std::unique_ptr<syntax::ast::ASTLiteralExpr> 
syntax::ExpressionParser::parseLiteralExpr(){
    const auto& token{ tokenConsumer.getToken() };
    tokenConsumer.consume(syntax::TokenType::LITERAL);
    if(token.value.back() == 'u'){
        return std::make_unique<syntax::ast::ASTLiteralExpr>(token, types::Type::UNSIGNED);
    }
    return std::make_unique<syntax::ast::ASTLiteralExpr>(token, types::Type::INT);
}

std::unique_ptr<syntax::ast::ASTBinaryExpr> 
syntax::ExpressionParser::parseOperator(){
    const auto& token{ tokenConsumer.getToken() };
    std::unique_ptr<syntax::ast::ASTBinaryExpr> op{ 
        std::make_unique<syntax::ast::ASTBinaryExpr>(token)
    };
    op->setOperator(syntax::tokenTypeToOperator(token.type));

    tokenConsumer.consume(syntax::GeneralTokenType::OPERATOR);

    return op;
}