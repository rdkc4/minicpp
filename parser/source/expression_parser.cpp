#include "../expression_parser.hpp"
#include "../token_consumer.hpp"

#include <format>
#include <memory>

ExpressionParser::ExpressionParser(TokenConsumer& consumer) : tokenConsumer{ consumer } {}

/** 
 * organized in a tree as reverse polish notation (RPN)
 * ast is built on the run (per subexpression)
*/
std::unique_ptr<ASTExpr> ExpressionParser::parseNumericalExpr(){
    // expression captures (subexpression) so there is no need to handle LPAREN and RPAREN
    std::unique_ptr<ASTExpr> expr{ parseExpr() };

    // holding the expression
    std::stack<std::unique_ptr<ASTExpr>> rpn;
    // handling the order of the operators
    std::stack<std::unique_ptr<ASTBinaryExpr>> weakOps;
    
    rpn.push(std::move(expr));
    // if there are no operators, this part will be skipped and expression will be returned
    while(tokenConsumer.getToken().gtype == GeneralTokenType::OPERATOR){
        std::unique_ptr<ASTBinaryExpr> op{ parseOperator() };

        expr = parseExpr();
        rpn.push(std::move(expr));

        weakOps.push(std::move(op));
        // if stronger operator comes up next, weak operators still need to wait
        while(getPrecedence(weakOps.top()->getToken().value) < getPrecedence(tokenConsumer.getToken().value)){
            std::unique_ptr<ASTBinaryExpr> nextOp{ parseOperator() };

            std::unique_ptr<ASTExpr> nextExpr{ parseExpr() };
            rpn.push(std::move(nextExpr));
            
            if(getPrecedence(nextOp->getToken().value) < getPrecedence(tokenConsumer.getToken().value)){
                weakOps.push(std::move(nextOp));
            }
            else{
                rpn.push(std::move(nextOp));
            }

        }
        // if operator of weaker precedence comes next, retrieve operators that have equal or stronger precedence
        while(!weakOps.empty() && 
            getPrecedence(weakOps.top()->getToken().value) >= 
            getPrecedence(tokenConsumer.getToken().value)
        ){
            rpn.push(std::move(weakOps.top()));
            weakOps.pop();
        }
    }

    std::unique_ptr<ASTExpr> numericalExprRoot{ std::move(rpn.top()) };
    rpn.pop();

    return rpnToTree(rpn, std::move(numericalExprRoot));
}

/** 
 * recursive top-down tree building from rpn based stack
 * child is not binary expression - leaf
 * if parent has children - subtree handled
*/
std::unique_ptr<ASTExpr> ExpressionParser::rpnToTree(std::stack<std::unique_ptr<ASTExpr>>& rpn, std::unique_ptr<ASTExpr> root) const {
    switch(root->getNodeType()){
        case ASTNodeType::ID_EXPR:
        case ASTNodeType::LITERAL_EXPR:
        case ASTNodeType::FUNCTION_CALL_EXPR:
            return root;
        case ASTNodeType::BINARY_EXPR: {
            auto* binaryExpr{ static_cast<ASTBinaryExpr*>(root.get()) };
            if(binaryExpr->initialized()){
                return root;
            }

            auto popNode = [&]() -> std::unique_ptr<ASTExpr> {
                auto node{ std::move(rpn.top()) };
                rpn.pop();
                return node;
            };

            auto rightSubtree{ 
                rpnToTree(rpn, popNode()) 
            };
            auto leftSubtree{ 
                rpnToTree(rpn, popNode()) 
            };

            binaryExpr->setOperandExprs(
                std::move(leftSubtree), 
                std::move(rightSubtree)
            );
            break;
        }
        default:
            break;
    }

    return root;
}

std::unique_ptr<ASTExpr> ExpressionParser::parseExpr(){
    const auto& token{ tokenConsumer.getToken() };
    switch(token.type){
        case TokenType::_LITERAL:
            return parseLiteralExpr();

        case TokenType::_ID:
            if(tokenConsumer.peek().type == TokenType::_LPAREN){
                return parseFunctionCallExpr();
            }
            return parseIdExpr();

        case TokenType::_LPAREN:{
            tokenConsumer.consume(TokenType::_LPAREN);
            auto expr{ parseNumericalExpr() };
            tokenConsumer.consume(TokenType::_RPAREN);
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

std::unique_ptr<ASTExpr> ExpressionParser::parseRelationalExpr(){
    std::unique_ptr<ASTExpr> leftOperand{ parseNumericalExpr() };
    std::unique_ptr<ASTBinaryExpr> relationalExpr{ parseOperator(true) };
    relationalExpr->setOperandExprs(
        std::move(leftOperand), 
        parseNumericalExpr()
    );
    return relationalExpr;
}

std::unique_ptr<ASTFunctionCallExpr> ExpressionParser::parseFunctionCallExpr(){
    std::unique_ptr<ASTFunctionCallExpr> callExpr{ 
        std::make_unique<ASTFunctionCallExpr>(tokenConsumer.getToken()) 
    };
    tokenConsumer.consume(TokenType::_ID);
    
    tokenConsumer.consume(TokenType::_LPAREN);
    parseArguments(callExpr.get());
    tokenConsumer.consume(TokenType::_RPAREN);
    
    return callExpr;
}

void ExpressionParser::parseArguments(ASTFunctionCallExpr* callExpr){
    if(tokenConsumer.getToken().type == TokenType::_RPAREN){
        return;
    }

    callExpr->addArgument(parseNumericalExpr());
    while(tokenConsumer.getToken().type == TokenType::_COMMA){
        tokenConsumer.consume(TokenType::_COMMA);
        callExpr->addArgument(parseNumericalExpr());
    }
}

std::unique_ptr<ASTIdExpr> ExpressionParser::parseIdExpr(){
    const auto& token{ tokenConsumer.getToken() };
    tokenConsumer.consume(TokenType::_ID);

    return std::make_unique<ASTIdExpr>(token);
}

std::unique_ptr<ASTLiteralExpr> ExpressionParser::parseLiteralExpr(){
    const auto& token{ tokenConsumer.getToken() };
    tokenConsumer.consume(TokenType::_LITERAL);
    if(token.value.back() == 'u'){
        return std::make_unique<ASTLiteralExpr>(token, Type::UNSIGNED);
    }
    return std::make_unique<ASTLiteralExpr>(token, Type::INT);
}

std::unique_ptr<ASTBinaryExpr> ExpressionParser::parseOperator(bool isRel){
    std::unique_ptr<ASTBinaryExpr> op{ 
        std::make_unique<ASTBinaryExpr>(Token{ tokenConsumer.getToken() })
    };
    if(stringToOperator.find(tokenConsumer.getToken().value) != stringToOperator.end()){
        op->setOperator(stringToOperator.at(tokenConsumer.getToken().value));
    }

    if(isRel){
        tokenConsumer.consume(TokenType::_RELOP);
    }
    else{
        tokenConsumer.consume(GeneralTokenType::OPERATOR);
    }

    return op;
}