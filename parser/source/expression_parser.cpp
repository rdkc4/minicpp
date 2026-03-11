#include "../expression_parser.hpp"
#include "../token_consumer.hpp"

#include <format>

ExpressionParser::ExpressionParser(TokenConsumer& consumer) : tokenConsumer{ consumer } {}

/** 
 * organized in a tree as reverse polish notation (RPN)
 * ast is built on the run (per subexpression)
*/
std::unique_ptr<ASTExpr> ExpressionParser::parseNumericalExpr(){
    // expression captures (subexpression) so there is no need to handle LPAREN and RPAREN
    std::unique_ptr<ASTExpr> expr = parseExpr();

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
        while(!weakOps.empty() && getPrecedence(weakOps.top()->getToken().value) >= getPrecedence(tokenConsumer.getToken().value)){
            rpn.push(std::move(weakOps.top()));
            weakOps.pop();
        }
    }

    std::unique_ptr<ASTExpr> numericalExprRoot{ std::move(rpn.top()) };
    rpn.pop();

    return rpnToTree(rpn, numericalExprRoot);
}

/** 
 * recursive top-down tree building from rpn based stack
 * child is not binary expression - leaf
 * if parent has children - subtree handled
*/
std::unique_ptr<ASTExpr> ExpressionParser::rpnToTree(std::stack<std::unique_ptr<ASTExpr>>& rpn, std::unique_ptr<ASTExpr>& root) const {
    if(root->getNodeType() == ASTNodeType::ID || root->getNodeType() == ASTNodeType::LITERAL || root->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return std::move(root);
    }
    else if(auto* binExpr = static_cast<ASTBinaryExpr*>(root.get())){
        if(binExpr->initialized()){
            return std::move(root);
        }

        std::unique_ptr<ASTExpr> rightSubtreeRoot = std::move(rpn.top());
        rpn.pop();
        std::unique_ptr<ASTExpr> rightSubtree = rpnToTree(rpn,rightSubtreeRoot);

        std::unique_ptr<ASTExpr> leftSubtreeRoot = std::move(rpn.top());
        rpn.pop();
        std::unique_ptr<ASTExpr> leftSubtree = rpnToTree(rpn, leftSubtreeRoot);

        binExpr->setOperands(std::move(leftSubtree), std::move(rightSubtree));
    }

    return std::move(root);
}

std::unique_ptr<ASTExpr> ExpressionParser::parseExpr(){
    if(tokenConsumer.getToken().type == TokenType::_LITERAL){
        return parseLiteralExpr();
    }
    else if(tokenConsumer.getToken().type == TokenType::_ID && tokenConsumer.peek().type == TokenType::_LPAREN){
        return parseFunctionCallExpr();
    }
    else if(tokenConsumer.getToken().type == TokenType::_ID){
        return parseIdExpr();    
    }
    else if(tokenConsumer.getToken().type == TokenType::_LPAREN){
        tokenConsumer.consume(TokenType::_LPAREN);
        std::unique_ptr<ASTExpr> numericalExpr = parseNumericalExpr();
        tokenConsumer.consume(TokenType::_RPAREN);
        return numericalExpr;
    }
    auto token = tokenConsumer.getToken();
    throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> expected 'EXPRESSION', got '{} {}'",
            token.line, token.column, tokenTypeToString.at(token.type), token.value));
}

std::unique_ptr<ASTExpr> ExpressionParser::parseRelationalExpr(){
    std::unique_ptr<ASTExpr> leftOperand = parseNumericalExpr();
    std::unique_ptr<ASTBinaryExpr> relationalExpr = parseOperator(true);
    relationalExpr->setOperands(std::move(leftOperand), parseNumericalExpr());
    return relationalExpr;
}

std::unique_ptr<ASTFunctionCallExpr> ExpressionParser::parseFunctionCallExpr(){
    std::unique_ptr<ASTFunctionCallExpr> callExpr = std::make_unique<ASTFunctionCallExpr>(tokenConsumer.getToken(), ASTNodeType::FUNCTION_CALL);
    tokenConsumer.consume(TokenType::_ID);
    
    tokenConsumer.consume(TokenType::_LPAREN);
    parseArguments(callExpr.get());
    tokenConsumer.consume(TokenType::_RPAREN);
    
    return callExpr;
}

void ExpressionParser::parseArguments(ASTFunctionCallExpr* callExpr){
    while(tokenConsumer.getToken().type != TokenType::_RPAREN){
        callExpr->addArgument(parseNumericalExpr());
        if(tokenConsumer.getToken().type == TokenType::_COMMA && tokenConsumer.peek().type != TokenType::_RPAREN){
            tokenConsumer.consume(TokenType::_COMMA);
        }
        else{
            break;
        }
    }
}

std::unique_ptr<ASTIdExpr> ExpressionParser::parseIdExpr(){
    const Token token{ tokenConsumer.getToken() };
    tokenConsumer.consume(TokenType::_ID);

    return std::make_unique<ASTIdExpr>(token, ASTNodeType::ID);
}

std::unique_ptr<ASTLiteralExpr> ExpressionParser::parseLiteralExpr(){
    const Token token{ tokenConsumer.getToken() };
    tokenConsumer.consume(TokenType::_LITERAL);
    if(token.value.back() == 'u'){
        return std::make_unique<ASTLiteralExpr>(token, ASTNodeType::LITERAL, Type::UNSIGNED);
    }
    return std::make_unique<ASTLiteralExpr>(token, ASTNodeType::LITERAL, Type::INT);
}

std::unique_ptr<ASTBinaryExpr> ExpressionParser::parseOperator(bool isRel){
    std::unique_ptr<ASTBinaryExpr> op =  std::make_unique<ASTBinaryExpr>(Token{ tokenConsumer.getToken() }, ASTNodeType::BINARY_EXPRESSION);
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