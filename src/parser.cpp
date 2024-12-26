#include "../include/parser.hpp"

Parser::Parser(Lexer& lexer, ScopeManager& scopeManager) : lexer(lexer), scopeManager(scopeManager), currentToken(lexer.nextToken()) {
    returnType = Types::NO_TYPE;
}

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

void Parser::parseProgram(){
    root = std::make_shared<ASTree>(ASTNodeType::PROGRAM, Token("program", 0, 0));

    root->pushChild(functionList());
    if(currentToken.type != TokenType::_EOF){
        throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
            + " -> SYNTAX ERROR near: " + currentToken.value);
    }

    root->traverse(1);
    semanticCheck(root);
}

void Parser::eat(TokenType type){
    if(currentToken.type == type){
        currentToken = lexer.nextToken();
    } 
    else{
        throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
            + " -> SYNTAX ERROR near: " + currentToken.value);
    }
}

std::shared_ptr<ASTree> Parser::functionList(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::FUNCTION_LIST, Token("function_list", 0, 0)); 
    while(currentToken.type == TokenType::_TYPE){
        currentNode->pushChild(function());
    }
    return currentNode;
}

std::shared_ptr<ASTree> Parser::function(){
    returnType = getTypeFromToken(currentToken);
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

std::shared_ptr<ASTree> Parser::parameter(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::PARAMETER, Token());
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

std::shared_ptr<ASTree> Parser::body(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::BODY, Token("body", currentToken.line, currentToken.column));
    eat(TokenType::_LBRACKET);
    currentNode->pushChild(variableList());
    currentNode->pushChild(statementList());
    eat(TokenType::_RBRACKET);
    return currentNode;
}

std::shared_ptr<ASTree> Parser::variableList(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::VARIABLE_LIST, Token("var_list", currentToken.line, currentToken.column));
    while(currentToken.type == TokenType::_TYPE){
        currentNode->pushChild(variable());
    }
    return currentNode;
}

std::shared_ptr<ASTree> Parser::variable(){
    Types type = getTypeFromToken(currentToken);
    eat(TokenType::_TYPE);
    auto token = currentToken;
    eat(TokenType::_ID);
    eat(TokenType::_SEMICOLON);

    return std::make_shared<ASTree>(ASTNodeType::VARIABLE, Token(token), type);
}

std::shared_ptr<ASTree> Parser::statementList(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::STATEMENT_LIST, Token("statement_list", currentToken.line, currentToken.column));
    while(currentToken.type != TokenType::_RBRACKET){
        currentNode->pushChild(statement());
    }
    return currentNode;
}

std::shared_ptr<ASTree> Parser::statement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::STATEMENT, Token("statement", currentToken.line, currentToken.column));
    if(lexer.peekAtNext().type == TokenType::_ASSIGN){
        currentNode->pushChild(assignmentStatement());
    }
    else if(currentToken.type == TokenType::_RETURN){
        currentNode->pushChild(returnStatement());
    }
    else if(currentToken.type == TokenType::_IF){
        currentNode->pushChild(ifStatement());
    }
    else if(currentToken.type == TokenType::_LBRACKET){
        currentNode->pushChild(compoundStatement());
    }
    else{
        throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
            + " -> SYNTAX ERROR near: " + currentToken.value);
    }
    return currentNode;
}

std::shared_ptr<ASTree> Parser::compoundStatement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::COMPOUND_STATEMENT, Token("cpnd_statement", currentToken.line, currentToken.column));

    eat(TokenType::_LBRACKET);
    currentNode->pushChild(statementList());
    eat(TokenType::_RBRACKET);
    
    return currentNode;
}

std::shared_ptr<ASTree> Parser::assignmentStatement(){
    if(currentToken.type == TokenType::_ID){
        auto lchild = std::make_shared<ASTree>(ASTNodeType::VARIABLE, Token(currentToken)); //add type
        eat(TokenType::_ID);
        auto currentNode = std::make_shared<ASTree>(ASTNodeType::ASSIGNMENT_STATEMENT, Token("=", currentToken.line, currentToken.column));
        eat(TokenType::_ASSIGN);
        auto rchild = numericalExpression();
        eat(TokenType::_SEMICOLON);
        
        currentNode->pushChild(rchild);
        currentNode->pushChild(lchild);
        return currentNode;
    }
    else{
        throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
            + " -> SYNTAX ERROR near: " + currentToken.value);
    }
}

std::shared_ptr<ASTree> Parser::returnStatement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::RETURN_STATEMENT, Token("return_statement", currentToken.line, currentToken.column));
    if(currentToken.type == TokenType::_RETURN){
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
    else{
        throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
            + " -> SYNTAX ERROR near: " + currentToken.value);
    }
}

std::shared_ptr<ASTree> Parser::ifStatement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::IF_STATEMENT, Token("ifstat", currentToken.line, currentToken.column));
    eat(TokenType::_IF);
    eat(TokenType::_LPAREN);
    currentNode->pushChild(relationalExpression());
    auto nextNode = currentNode->getChildren().back();
    eat(TokenType::_RPAREN);
    
    nextNode->pushChild(statement());
    if(currentToken.type == TokenType::_ELSE){
        eat(TokenType::_ELSE);
        nextNode->pushChild(statement());
    }
    
    return currentNode;
}

std::shared_ptr<ASTree> Parser::numericalExpression(){
    auto child = expression();
    
    if(currentToken.type != TokenType::_AROP){
        return child;
    }

    auto currentNode = std::make_shared<ASTree>(ASTNodeType::NUMERICAL_EXPRESSION, Token("dummyNode", currentToken.line, currentToken.column));
    auto tempNode = currentNode;

    while(currentToken.type == TokenType::_AROP){
        tempNode->pushChild(std::make_shared<ASTree>(ASTNodeType::NUMERICAL_EXPRESSION, Token(currentToken)));
        tempNode = tempNode->getChildren().back();
        tempNode->pushChild(child);
        eat(TokenType::_AROP);
        child = expression();
        if(currentToken.type != TokenType::_AROP){
            tempNode->pushChild(child);
        }
    }
    
    return currentNode->getChildren().back();
}

std::shared_ptr<ASTree> Parser::expression(){
    if(currentToken.type == TokenType::_LITERAL){
        auto token = currentToken;
        eat(TokenType::_LITERAL);
        return std::make_shared<ASTree>(ASTNodeType::LITERAL, token, Types::INT); //temporary
    }
    else if(currentToken.type == TokenType::_ID && lexer.peekAtNext().type == TokenType::_LPAREN){
        return functionCall();
    }
    else if(currentToken.type == TokenType::_ID){
        auto token = currentToken;
        eat(TokenType::_ID);
        return std::make_shared<ASTree>(ASTNodeType::ID, token, Types::INT); // temporary
    }
    else if(currentToken.type == TokenType::_LPAREN){
        eat(TokenType::_LPAREN);
        auto node = numericalExpression();
        eat(TokenType::_RPAREN);
        return node;
    }
    throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
        + " -> SYNTAX ERROR near: " + currentToken.value);
}

std::shared_ptr<ASTree> Parser::relationalExpression(){
    auto lchild = numericalExpression();
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::RELATIONAL_EXPRESSION, currentToken);
    eat(TokenType::_RELOP);
    currentNode->pushChild(lchild);
    currentNode->pushChild(numericalExpression());
    return currentNode;
}

std::shared_ptr<ASTree> Parser::functionCall(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::FUNCTION_CALL, currentToken);
    eat(TokenType::_ID);
    eat(TokenType::_LPAREN);
    currentNode->pushChild(argument());
    eat(TokenType::_RPAREN);
    return currentNode;
}

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

//------------------------------------------------------------------------------------------------------------------
//should be in separate class
void Parser::semanticCheck(std::shared_ptr<ASTree> root){
    auto flist = root;
    flist = flist->getChildren().back();
    scopeManager.pushScope();

    for(const auto& child : flist->getChildren()){
        scopeManager.pushSymbol(Symbol(child->getToken()->value, Kinds::FUN, child->getType().value(), 0, 0));
    }

    if(!scopeManager.getSymbolTable().lookupSymbol("main", {Kinds::FUN})){
        throw std::runtime_error("'main' function not found");
    }
    for(const auto& child : flist->getChildren()){
        functionCheck(child);
    }
    scopeManager.popScope();
}

void Parser::functionCheck(std::shared_ptr<ASTree> node){
    scopeManager.pushScope();
    node->getToken();
    scopeManager.popScope();
}

void Parser::parameterCheck(std::shared_ptr<ASTree> node){
    for(const auto& child : node->getChildren()){
        if(!scopeManager.pushSymbol(Symbol(child->getToken()->value, Kinds::PAR, child->getType().value(), 0, 0))){
            throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
                + ": SEMANTIC ERROR -> redefined variable " + node->getToken()->value);
        }
    }
}

/*void bodyCheck(std::shared_ptr<ASTree> node);

void checkVariables(std::shared_ptr<ASTree> node);

void checkStatements(std::shared_ptr<ASTree> node){
    for(const auto& child : node->getChildren()){
        checkStatement(child);
    }
}

void checkStatement(std::shared_ptr<ASTree> node){

}

void checkNumexp(std::shared_ptr<ASTree> node);
*/

void Parser::checkRelexp(std::shared_ptr<ASTree> node){
    auto lchild = node->getChild(0);
    auto rchild = node->getChild(1);
    if(lchild->getToken()->type == TokenType::_ID){
        checkID(lchild);
    }
    if(rchild->getToken()->type == TokenType::_ID){
        checkID(rchild);
    }
    if((lchild->getType() != rchild->getType()) || !lchild->getType().has_value()){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ": SEMANTIC ERROR -> type mismatch " + node->getToken()->value);
    }
}

void Parser::checkID(std::shared_ptr<ASTree> node){
    if(!scopeManager.getSymbolTable().lookupSymbol(node->getToken()->value, {Kinds::VAR, Kinds::PAR})){
        throw std::runtime_error("Line " + std::to_string(node->getToken()->line) + " Column " + std::to_string(node->getToken()->column)
            + ":SEMANTIC ERROR -> undefined variable " + node->getToken()->value);
    }
}

/*
void Parser::checkArgument
*/