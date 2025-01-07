#include "parser.hpp"

#include <memory>
#include <stdexcept>
#include <format>

Parser::Parser(Lexer& lexer) : lexer(lexer), currentToken(lexer.nextToken()) {}


//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETRIEVAL OF A TYPE
//-----------------------------------------------------------------------------------------------------------------------------------------------------
Types Parser::getTypeFromToken(Token token){
    if(token.value == "int"){
        return Types::INT;
    }
    else if(token.value == "unsigned"){
        return Types::UNSIGNED;
    }
    else if(token.value == "void"){
        return Types::VOID;
    }
    else{
        return Types::NO_TYPE;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SYNTAX CHECK
// ABSTRACT SYNTAX TREE BUILDING
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::parseProgram(){
    std::shared_ptr<ASTree> root = std::make_shared<ASTree>(ASTNodeType::PROGRAM, Token("program", 0, 0));
    root->pushChild(functionList());
    if(currentToken.type != TokenType::_EOF){
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> near '{}'",
            currentToken.line, currentToken.column, currentToken.value));
    }
    return root;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// TOKEN CONSUMPTION - ensuring order of tokens is right
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void Parser::eat(TokenType type){
    if(currentToken.type == type){
        currentToken = lexer.nextToken();
    } 
    else{
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> near '{}'",
            currentToken.line, currentToken.column, currentToken.value));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION_LIST : FUNCTION | FUNCTION_LIST FUNCTION
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::functionList(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::FUNCTION_LIST, Token("function_list", 0, 0)); 
    while(currentToken.type == TokenType::_TYPE){
        currentNode->pushChild(function());
    }
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION : TYPE ID LPAREN PARAMETER(0+) RPAREN BODY
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::function(){
    auto returnType = getTypeFromToken(currentToken);
    eat(TokenType::_TYPE);
    auto token = currentToken;
    eat(TokenType::_ID);
    
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::FUNCTION, Token(token), returnType);

    eat(TokenType::_LPAREN);
    currentNode->pushChild(parameter());
    eat(TokenType::_RPAREN);
    currentNode->pushChild(body());
    
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// PARAMETERS : EMPTY | PARAMETER | PARAMETERS PARAMETER
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::parameter(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::PARAMETER, Token("params", currentToken.line, currentToken.column));
    while(currentToken.type == TokenType::_TYPE){
        Types type = getTypeFromToken(currentToken);
        eat(TokenType::_TYPE);
        auto token = currentToken;
        eat(TokenType::_ID);
        
        currentNode->pushChild(std::make_shared<ASTree>(ASTNodeType::PARAMETER, Token(token), type));

        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().type == TokenType::_TYPE){
            eat(TokenType::_COMMA);
        }
    }
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// BODY : LBRACKET VARIABLE_LIST(0+) STATEMENT_LIST RBRACKET
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::body(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::BODY, Token("body", currentToken.line, currentToken.column));
    eat(TokenType::_LBRACKET);
    currentNode->pushChild(variableList());
    currentNode->pushChild(statementList());
    eat(TokenType::_RBRACKET);
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLE_LIST : EMPTY | VARIABLE | VARIABLE_LIST VARIABLE
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::variableList(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::VARIABLE_LIST, Token("var_list", currentToken.line, currentToken.column));
    while(currentToken.type == TokenType::_TYPE){
        currentNode->pushChild(variable());
    }
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// VARIABLE : TYPE ID SEMICOLON
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::variable(){
    Types type = getTypeFromToken(currentToken);
    eat(TokenType::_TYPE);
    auto token = currentToken;
    eat(TokenType::_ID);
    eat(TokenType::_SEMICOLON);

    return std::make_shared<ASTree>(ASTNodeType::VARIABLE, Token(token), type);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// STATEMENT_LIST : EMPTY | STATEMENT | STATEMENT_LIST STATEMENT
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::statementList(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::STATEMENT_LIST, Token("statement_list", currentToken.line, currentToken.column));
    while(currentToken.type != TokenType::_RBRACKET){
        currentNode->pushChild(statement());
    }
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// STATEMENT TYPES
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::statement(){
    if(lexer.peekAtNext().type == TokenType::_ASSIGN){
        auto node = assignmentStatement();
        eat(TokenType::_SEMICOLON);
        return node;
    }
    else if(currentToken.type == TokenType::_RETURN)
        return returnStatement();
    else if(currentToken.type == TokenType::_IF)
        return ifStatement();
    else if(currentToken.type == TokenType::_LBRACKET)
        return compoundStatement();
    else if(currentToken.type == TokenType::_WHILE)
        return whileStatement();
    else if(currentToken.type == TokenType::_FOR)
        return forStatement();
    else if(currentToken.type == TokenType::_DO)
        return doWhileStatement();
    else if(currentToken.type == TokenType::_SWITCH)
        return switchStatement();
    else
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> near '{}'",
            currentToken.line, currentToken.column, currentToken.value));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// COMPOUND_STATEMENT : LBRACKET STATEMENT_LIST RBRACKET
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::compoundStatement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::COMPOUND_STATEMENT, Token("cpnd_statement", currentToken.line, currentToken.column));

    eat(TokenType::_LBRACKET);
    currentNode->pushChild(statementList());
    eat(TokenType::_RBRACKET);
    
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ASSIGNMENT_STATEMENT : ID ASSIGN NUMERICAL_EXPRESSION
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::assignmentStatement(){
    if(currentToken.type == TokenType::_ID){
        auto lchild = std::make_shared<ASTree>(ASTNodeType::VARIABLE, Token(currentToken));
        eat(TokenType::_ID);
        auto currentNode = std::make_shared<ASTree>(ASTNodeType::ASSIGNMENT_STATEMENT, Token("=", currentToken.line, currentToken.column));
        eat(TokenType::_ASSIGN);
        auto rchild = numericalExpression();
        
        currentNode->pushChild(lchild);
        currentNode->pushChild(rchild);
        return currentNode;
    }
    else{
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> near '{}'",
            currentToken.line, currentToken.column, currentToken.value));
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN_STATEMENT : RETURN SEMICOLON | RETURN NUMERICAL_EXPRESSION SEMICOLON
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::returnStatement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::RETURN_STATEMENT, Token("return_statement", currentToken.line, currentToken.column));
    eat(TokenType::_RETURN);

    if(currentToken.type == TokenType::_SEMICOLON){
        currentNode->setType(Types::VOID);
        eat(TokenType::_SEMICOLON);
    }
    else{
        currentNode->pushChild(numericalExpression());
        eat(TokenType::_SEMICOLON);
    }

    return currentNode;   
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// IF_STATEMENT : IF LPAREN RELATIONAL_EXPRESSION RPAREN STATEMENT | IF_STATEMENT ELSE STATEMENT | IF_STATEMENT ELSE IF_STATEMENT
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::ifStatement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::IF_STATEMENT, Token("if_stat", currentToken.line, currentToken.column));
    eat(TokenType::_IF);
    eat(TokenType::_LPAREN);
    currentNode->pushChild(relationalExpression());
    eat(TokenType::_RPAREN);
    
    currentNode->pushChild(statement());

    while(currentToken.type == TokenType::_ELSE && lexer.peekAtNext().type == TokenType::_IF){
        eat(TokenType::_ELSE);
        eat(TokenType::_IF);

        eat(TokenType::_LPAREN);
        currentNode->pushChild(relationalExpression());
        eat(TokenType::_RPAREN);
        
        currentNode->pushChild(statement());
    }

    if(currentToken.type == TokenType::_ELSE){
        eat(TokenType::_ELSE);
        currentNode->pushChild(statement());
    }
    
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// WHILE_STATEMENT : WHILE LPAREN RELATIONAL_EXPRESSION RPAREN STATEMENT
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::whileStatement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::WHILE_STATEMENT, Token("while_stat", currentToken.line, currentToken.column));
    eat(TokenType::_WHILE);
    
    eat(TokenType::_LPAREN);
    currentNode->pushChild(relationalExpression());
    eat(TokenType::_RPAREN);
    
    currentNode->pushChild(statement());

    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FOR_STATEMENT : FOR LPAREN ASSIGN_STATEMENT SEMICOLON RELATIONAL_EXPRESSION SEMICOLON ASSIGNMENT_STATEMENT RPAREN STATEMENT
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::forStatement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::FOR_STATEMENT, Token("for_stat", currentToken.line, currentToken.column));
    eat(TokenType::_FOR);

    eat(TokenType::_LPAREN);
    currentNode->pushChild(assignmentStatement());
    eat(TokenType::_SEMICOLON);
    currentNode->pushChild(relationalExpression());
    eat(TokenType::_SEMICOLON);
    currentNode->pushChild(assignmentStatement());
    eat(TokenType::_RPAREN);

    currentNode->pushChild(statement());
    
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DO_WHILE_STATEMENT : DO STATEMENT WHILE LPAREN RELATIONAL_EXPRESSION RPAREN SEMICOLON
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::doWhileStatement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::DO_WHILE_STATEMENT, Token("dowhile_stat", currentToken.line, currentToken.column));
    eat(TokenType::_DO);
    currentNode->pushChild(statement());
    eat(TokenType::_WHILE);

    eat(TokenType::_LPAREN);
    currentNode->pushChild(relationalExpression());
    eat(TokenType::_RPAREN);
    eat(TokenType::_SEMICOLON);

    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SWITCH_STATEMENT : SWITCH LPAREN ID RPAREN LBRACKET CASE(1+) DEFAULT(0-1) RBRACKET
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::switchStatement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::SWITCH_STATEMENT, Token("switch_stat", currentToken.line, currentToken.column));
    eat(TokenType::_SWITCH);

    eat(TokenType::_LPAREN);
    currentNode->pushChild(id());
    eat(TokenType::_RPAREN);

    eat(TokenType::_LBRACKET);
    
    currentNode->pushChild(_case());
    while(currentToken.type == TokenType::_CASE){
        currentNode->pushChild(_case());
    }
    
    if(currentToken.type == TokenType::_DEFAULT){
        currentNode->pushChild(_default());
    }

    eat(TokenType::_RBRACKET);
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SWITCH_CASE_STATEMENT_LIST : STATEMENT | SWITCH_CASE_STATEMENT_LIST STATEMENT
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::switchCaseStatementList(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::STATEMENT_LIST, Token("statement_list", currentToken.line, currentToken.column));
    while(currentToken.type != TokenType::_CASE && currentToken.type != TokenType::_DEFAULT && 
            currentToken.type != TokenType::_RBRACKET && currentToken.type != TokenType::_BREAK){
        
        currentNode->pushChild(statement());
    }

    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// _CASE_ : CASE LITERAL COLON SWITCH_CASE_STATEMENT_LIST BREAK(0-1) 
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::_case(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::CASE, Token("case", currentToken.line, currentToken.column));
    eat(TokenType::_CASE);
    currentNode->pushChild(literal());
    eat(TokenType::_LITERAL);
    eat(TokenType::_COLON);
    
    currentNode->pushChild(switchCaseStatementList());
    if(currentToken.type == TokenType::_BREAK){
        currentNode->pushChild(_break());
    }

    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// _DEFAULT_ : DEFAULT COLON SWITCH_CASE_STATEMENT_LIST BREAK(0-1)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::_default(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::DEFAULT, Token("default", currentToken.line, currentToken.column));
    eat(TokenType::_DEFAULT);
    eat(TokenType::_COLON);
    
    currentNode->pushChild(switchCaseStatementList());
    if(currentToken.type == TokenType::_BREAK){
        eat(TokenType::_BREAK);
        eat(TokenType::_SEMICOLON);
    }

    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// _BREAK_ : BREAK SEMICOLON
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::_break(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::BREAK, Token("break", currentToken.line, currentToken.column));
    eat(TokenType::_BREAK);
    eat(TokenType::_SEMICOLON);
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// NUMERICAL_EXPRESSION : EXPRESSION | NUMERICAL_EXPRESSION AROP EXPRESSION
// organized in a tree as reverse polish notation (RPN)
// helper method getPrecedence(operator)
// helper method rpnToTree(rpnStack, parentNode)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::numericalExpression(){
    auto child = expression();
    std::stack<std::shared_ptr<ASTree>> rpn;
    std::stack<std::shared_ptr<ASTree>> weakOperators;
    
    rpn.push(child);
    while(currentToken.type == TokenType::_AROP || currentToken.type == TokenType::_BITWISE){
        auto arOperator = std::make_shared<ASTree>(ASTNodeType::NUMERICAL_EXPRESSION, Token(currentToken));
        if(currentToken.type == TokenType::_BITWISE){
            eat(TokenType::_BITWISE);
        }else{
            eat(TokenType::_AROP);
        }
        child = expression();
        rpn.push(child);

        weakOperators.push(arOperator);
        while(getPrecedence(weakOperators.top()->getToken()->value) < getPrecedence(currentToken.value)){
            auto strongArOp = std::make_shared<ASTree>(ASTNodeType::NUMERICAL_EXPRESSION, Token(currentToken));
            if(currentToken.type == TokenType::_BITWISE){
                eat(TokenType::_BITWISE);
            }else{
                eat(TokenType::_AROP);
            }
            auto strongChild = expression();
            rpn.push(strongChild);
            if(getPrecedence(strongArOp->getToken()->value) < getPrecedence(currentToken.value))
                weakOperators.push(strongArOp);
            else
                rpn.push(strongArOp);
        }
        while(!weakOperators.empty() && getPrecedence(weakOperators.top()->getToken()->value) >= getPrecedence(currentToken.value)){
            rpn.push(weakOperators.top());
            weakOperators.pop();
        }
    }

    auto root = rpn.top();
    rpn.pop();

    return rpnToTree(rpn, root);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// determining the importance of the operator
// -> expand for more operators (todo)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
int Parser::getPrecedence(std::string& op){
    if(op == "|"){
        return 1;
    }
    else if(op == "^"){
        return 2;
    }
    else if(op == "&"){
        return 3;
    }
    else if(op == "<<" || op == ">>"){
        return 4;
    }
    else if(op == "-" || op == "+"){
        return 5;
    }
    else if (op == "*" || op == "/"){
        return 6;
    }
    else{
        return 0;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// recursive top-down tree building from rpn based stack
// if child is not numerical expression - leaf
// if parent has two children - subtree is handled
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::rpnToTree(std::stack<std::shared_ptr<ASTree>>& rpn, std::shared_ptr<ASTree> parent){
    if(parent->getNodeType() != ASTNodeType::NUMERICAL_EXPRESSION || !parent->getChildren().empty()){
        return parent;
    }
    auto r = rpn.top();
    rpn.pop();
    auto rchild = rpnToTree(rpn,r);

    auto l = rpn.top();
    rpn.pop();
    auto lchild = rpnToTree(rpn, l);

    parent->pushChild(lchild);
    parent->pushChild(rchild);

    return parent;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// EXPRESSION : LITERAL | FUNCTION_CALL | ID | LPAREN NUMERICAL_EXPRESSION RPAREN
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::expression(){
    if(currentToken.type == TokenType::_LITERAL){
        auto node = literal();
        eat(TokenType::_LITERAL);
        return node;
    }
    else if(currentToken.type == TokenType::_ID && lexer.peekAtNext().type == TokenType::_LPAREN){
        return functionCall();
    }
    else if(currentToken.type == TokenType::_ID){
        return id();    
    }
    else if(currentToken.type == TokenType::_LPAREN){
        eat(TokenType::_LPAREN);
        auto node = numericalExpression();
        eat(TokenType::_RPAREN);
        return node;
    }
    throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> near '{}'",
            currentToken.line, currentToken.column, currentToken.value));
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RELATIONAL EXPRESSION : NUMERICAL_EXPRESSION RELOP NUMERICAL_EXPRESSION
// -> rework for multiple conditions (TODO)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::relationalExpression(){
    auto lchild = numericalExpression();
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::RELATIONAL_EXPRESSION, currentToken);
    eat(TokenType::_RELOP);
    currentNode->pushChild(lchild);
    currentNode->pushChild(numericalExpression());
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION_CALL : ID LPAREN ARGUMENTS(0+) RPAREN
// -> anonymous functions (TODO)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::functionCall(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::FUNCTION_CALL, currentToken);
    eat(TokenType::_ID);
    eat(TokenType::_LPAREN);
    currentNode->pushChild(argument());
    eat(TokenType::_RPAREN);
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ARGUMENTS : EMPTY | NUMERICAL_EXPRESSION | ARGUMENTS COMMA NUMERICAL_EXPRESSION
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::argument(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::ARGUMENT, Token("arg", currentToken.line, currentToken.column));
    while(currentToken.type != TokenType::_RPAREN){
        currentNode->pushChild(numericalExpression());
        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().type != TokenType::_RPAREN){
            eat(TokenType::_COMMA);
        }
    }
    return currentNode;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ID : ID
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::id(){
    auto token = currentToken;
    eat(TokenType::_ID);

    return std::make_shared<ASTree>(ASTNodeType::ID, token);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// LITERAL : LITERAL(unsigned) | LITERAL(int)
// -> more types (TODO)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> Parser::literal(){
    if(currentToken.value.back() == 'u'){
        return std::make_shared<ASTree>(ASTNodeType::LITERAL, Token(currentToken), Types::UNSIGNED);
    }
    return std::make_shared<ASTree>(ASTNodeType::LITERAL, Token(currentToken), Types::INT);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// -> AUTO KEYWORD (TODO)
// -> GLOBAL VARIABLES (TODO)
// -> ENUMERATORS (TODO)
// -> LISTS (TODO)
// -> PRINT (TODO)
// -> MATCH KEYWORD (TODO)
// -> INCLUDE / HANDLING MULTIPLE FILES (TODO)
// -> PRE/POST INCREMENT/DECREMENT (TODO)
// -> BITWISE OPERATORS (!, ~) (TODO)
// -> TERNARY OPERATOR ? : (TODO)
// -> IN-PLACE INITIALIZATION (TODO)
//-----------------------------------------------------------------------------------------------------------------------------------------------------