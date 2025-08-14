#include "../expression_parser.hpp"
#include "../token_consumer.hpp"

ExpressionParser::ExpressionParser(Lexer& lexer) : TokenConsumer{ lexer } {}

// NUMERICAL_EXPRESSION : EXPRESSION 
//                      | NUMERICAL_EXPRESSION OPERATOR EXPRESSION
// organized in a tree as reverse polish notation (RPN)
// ast is built on the run (per subexpression)
std::unique_ptr<ASTExpression> ExpressionParser::numericalExpression(){
    // expression captures (subexpression) so there is no need to handle LPAREN and RPAREN
    std::unique_ptr<ASTExpression> _expression = expression();

    // holding the expression
    std::stack<std::unique_ptr<ASTExpression>> rpn;
    // handling the order of the operators
    std::stack<std::unique_ptr<ASTBinaryExpression>> weakOperators;
    
    rpn.push(std::move(_expression));
    // if there are no operators, this part will be skipped and expression will be returned
    while(getToken().gtype == GeneralTokenType::OPERATOR){
        std::unique_ptr<ASTBinaryExpression> _op{ _operator() };

        _expression = expression();
        rpn.push(std::move(_expression));

        weakOperators.push(std::move(_op));
        // if stronger operator comes up next, weak operators still need to wait
        while(getPrecedence(weakOperators.top()->getToken().value) < getPrecedence(getToken().value)){
            std::unique_ptr<ASTBinaryExpression> _nextOperator{ _operator() };

            std::unique_ptr<ASTExpression> _nextExpression{ expression() };
            rpn.push(std::move(_nextExpression));
            
            if(getPrecedence(_nextOperator->getToken().value) < getPrecedence(getToken().value)){
                weakOperators.push(std::move(_nextOperator));
            }
            else{
                rpn.push(std::move(_nextOperator));
            }

        }
        // if operator of weaker precedence comes next, retrieve operators that have equal or stronger precedence
        while(!weakOperators.empty() && getPrecedence(weakOperators.top()->getToken().value) >= getPrecedence(getToken().value)){
            rpn.push(std::move(weakOperators.top()));
            weakOperators.pop();
        }
    }

    std::unique_ptr<ASTExpression> _numericalExpRoot{ std::move(rpn.top()) };
    rpn.pop();

    return rpnToTree(rpn, _numericalExpRoot);
}

// recursive top-down tree building from rpn based stack
// child is not binary expression - leaf
// if parent has children - subtree handled
std::unique_ptr<ASTExpression> ExpressionParser::rpnToTree(std::stack<std::unique_ptr<ASTExpression>>& rpn, std::unique_ptr<ASTExpression>& root) const {
    if(root->getNodeType() == ASTNodeType::ID || root->getNodeType() == ASTNodeType::LITERAL || root->getNodeType() == ASTNodeType::FUNCTION_CALL){
        return std::move(root);
    }
    else if(auto binExp = static_cast<ASTBinaryExpression*>(root.get())){
        if(binExp->initialized()){
            return std::move(root);
        }

        std::unique_ptr<ASTExpression> _rightSubtreeRoot = std::move(rpn.top());
        rpn.pop();
        std::unique_ptr<ASTExpression> _rightSubtree = rpnToTree(rpn,_rightSubtreeRoot);

        std::unique_ptr<ASTExpression> _leftSubtreeRoot = std::move(rpn.top());
        rpn.pop();
        std::unique_ptr<ASTExpression> _leftSubtree = rpnToTree(rpn, _leftSubtreeRoot);

        binExp->setOperands(std::move(_leftSubtree), std::move(_rightSubtree));
    }

    return std::move(root);
}

// EXPRESSION : LITERAL 
//            | FUNCTION_CALL 
//            | ID 
//            | LPAREN NUMERICAL_EXPRESSION RPAREN
std::unique_ptr<ASTExpression> ExpressionParser::expression(){
    if(getToken().type == TokenType::_LITERAL){
        return literal();
    }
    else if(getToken().type == TokenType::_ID && peek().type == TokenType::_LPAREN){
        return functionCall();
    }
    else if(getToken().type == TokenType::_ID){
        return id();    
    }
    else if(getToken().type == TokenType::_LPAREN){
        consume(TokenType::_LPAREN);
        std::unique_ptr<ASTExpression> _numericalExp = numericalExpression();
        consume(TokenType::_RPAREN);
        return _numericalExp;
    }
    throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> expected 'EXPRESSION', got '{} {}'",
            getToken().line, getToken().column, tokenTypeToString.at(getToken().type), getToken().value));
}


// RELATIONAL EXPRESSION : NUMERICAL_EXPRESSION RELOP NUMERICAL_EXPRESSION
std::unique_ptr<ASTExpression> ExpressionParser::relationalExpression(){
    std::unique_ptr<ASTExpression> _leftOperand = numericalExpression();
    std::unique_ptr<ASTBinaryExpression> _relationalExpression = _operator(true);
    _relationalExpression->setOperands(std::move(_leftOperand), numericalExpression());
    return _relationalExpression;
}


// FUNCTION_CALL : ID LPAREN ARGUMENT RPAREN
std::unique_ptr<ASTFunctionCall> ExpressionParser::functionCall(){
    std::unique_ptr<ASTFunctionCall> _functionCall = std::make_unique<ASTFunctionCall>(getToken(), ASTNodeType::FUNCTION_CALL);
    consume(TokenType::_ID);
    
    consume(TokenType::_LPAREN);
    argument(_functionCall.get());
    consume(TokenType::_RPAREN);
    
    return _functionCall;
}

// ARGUMENT : EMPTY 
//          | NUMERICAL_EXPRESSION 
//          | ARGUMENT COMMA NUMERICAL_EXPRESSION
void ExpressionParser::argument(ASTFunctionCall* _functionCall){
    while(getToken().type != TokenType::_RPAREN){
        _functionCall->addArgument(numericalExpression());
        if(getToken().type == TokenType::_COMMA && peek().type != TokenType::_RPAREN){
            consume(TokenType::_COMMA);
        }
        else{
            break;
        }
    }
}

// ID : ID
std::unique_ptr<ASTId> ExpressionParser::id(){
    const Token token{ getToken() };
    consume(TokenType::_ID);

    return std::make_unique<ASTId>(token, ASTNodeType::ID);
}

// LITERAL : LITERAL(unsigned) | LITERAL(int)
std::unique_ptr<ASTLiteral> ExpressionParser::literal(){
    const Token token{ getToken() };
    consume(TokenType::_LITERAL);
    if(token.value.back() == 'u'){
        return std::make_unique<ASTLiteral>(token, ASTNodeType::LITERAL, Types::UNSIGNED);
    }
    return std::make_unique<ASTLiteral>(token, ASTNodeType::LITERAL, Types::INT);
}

// root of the binary expression node (only operator, operands are assigned later)
std::unique_ptr<ASTBinaryExpression> ExpressionParser::_operator(bool isRel){
    std::unique_ptr<ASTBinaryExpression> _op =  std::make_unique<ASTBinaryExpression>(Token{getToken()}, ASTNodeType::BINARY_EXPRESSION);
    if(stringToOperator.find(getToken().value) != stringToOperator.end()){
        _op->setOperator(stringToOperator.at(getToken().value));
    }

    if(isRel){
        consume(TokenType::_RELOP);
    }
    else{
        consume(GeneralTokenType::OPERATOR);
    }

    return _op;
}