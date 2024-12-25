#include "../include/parser.hpp"

Parser::Parser(Lexer& lexer, ScopeManager& scopeManager) : lexer(lexer), scopeManager(scopeManager), currentToken(lexer.nextToken()) {
    returnType = Types::NO_TYPE;
}

Types Parser::getType(Token token){
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
    scopeManager.pushScope();
    root = std::make_shared<ASTree>(ASTNodeType::PROGRAM, Token());

    root->pushChild(functionList());
    if(currentToken.type != TokenType::_EOF){
        throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
            + " -> SYNTAX ERROR near: " + currentToken.value);
    }
    
    //scopeManager.printSymbolTable();
    scopeManager.popScope();
    root->traverse();
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
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::FUNCTION_LIST, Token()); 
    while(currentToken.type == TokenType::_TYPE){
        currentNode->pushChild(function());
    }
    return currentNode;
}

std::shared_ptr<ASTree> Parser::function(){
    returnType = getType(currentToken);
    eat(TokenType::_TYPE);
    std::string name = currentToken.value;
    eat(TokenType::_ID);
    
    scopeManager.pushSymbol(Symbol(name, Kinds::FUN, returnType));
    scopeManager.pushScope();
    
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::FUNCTION, Token(), returnType);

    eat(TokenType::_LPAREN);
    currentNode->pushChild(parameter());
    eat(TokenType::_RPAREN);
    currentNode->pushChild(body());
    
    //scopeManager.printSymbolTable();
    scopeManager.popScope();
    return currentNode;
}

std::shared_ptr<ASTree> Parser::parameter(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::PARAMETER, Token());
    while(currentToken.type == TokenType::_TYPE){
        Types type = getType(currentToken);
        eat(TokenType::_TYPE);
        auto token = currentToken;
        eat(TokenType::_ID);
        
        scopeManager.pushSymbol(Symbol(token.value, Kinds::PAR, type));
        currentNode->pushChild(std::make_shared<ASTree>(ASTNodeType::PARAMETER, Token(token), type));

        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().type == TokenType::_TYPE){
            eat(TokenType::_COMMA);
        }
    }
    return currentNode;
}

std::shared_ptr<ASTree> Parser::body(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::BODY, Token());
    eat(TokenType::_LBRACKET);
    currentNode->pushChild(variableList());
    currentNode->pushChild(statementList());
    eat(TokenType::_RBRACKET);
    return currentNode;
}

std::shared_ptr<ASTree> Parser::variableList(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::VARIABLE_LIST, Token());
    while(currentToken.type == TokenType::_TYPE){
        currentNode->pushChild(variable());
    }
    return currentNode;
}

std::shared_ptr<ASTree> Parser::variable(){
    Types type = getType(currentToken);
    eat(TokenType::_TYPE);
    auto token = currentToken;
    eat(TokenType::_ID);
    eat(TokenType::_SEMICOLON);

    scopeManager.pushSymbol(Symbol(token.value, Kinds::VAR, type));
    return std::make_shared<ASTree>(ASTNodeType::VARIABLE, Token(token), type);
}

std::shared_ptr<ASTree> Parser::statementList(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::STATEMENT_LIST, Token());
    while(currentToken.type != TokenType::_RBRACKET){
        currentNode->pushChild(statement());
    }
    return currentNode;
}

std::shared_ptr<ASTree> Parser::statement(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::STATEMENT, Token());
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
    scopeManager.pushScope();
    
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::COMPOUND_STATEMENT, Token());

    eat(TokenType::_LBRACKET);
    currentNode->pushChild(statementList());
    eat(TokenType::_RBRACKET);
    
    scopeManager.popScope();
    return currentNode;
}

std::shared_ptr<ASTree> Parser::assignmentStatement(){
    if(currentToken.type == TokenType::_ID){
        auto lchild = std::make_shared<ASTree>(ASTNodeType::VARIABLE, Token(currentToken)); //add type
        eat(TokenType::_ID);
        auto currentNode = std::make_shared<ASTree>(ASTNodeType::ASSIGNMENT_STATEMENT, Token());
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
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::RETURN_STATEMENT, Token());
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
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::IF_STATEMENT, Token());
    eat(TokenType::_IF);
    eat(TokenType::_LPAREN);
    currentNode->pushChild(relationalExpression());
    auto nextNode = currentNode->getChildren().back();
    eat(TokenType::_RPAREN);
    scopeManager.pushScope();
    nextNode->pushChild(statement());
    if(currentToken.type == TokenType::_ELSE){
        eat(TokenType::_ELSE);
        nextNode->pushChild(statement());
    }
    scopeManager.popScope();
    return currentNode;
}

std::shared_ptr<ASTree> Parser::numericalExpression(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::NUMERICAL_EXPRESSION, Token());
    auto tempNode = currentNode;
    auto [valid, node] = expression();
    if(valid){
        currentNode->pushChild(node);
        while(currentToken.type == TokenType::_AROP){
            tempNode->pushChild(std::make_shared<ASTree>(ASTNodeType::NUMERICAL_EXPRESSION, Token()));
            tempNode = tempNode->getChildren().back();
            eat(TokenType::_AROP);
            auto [exprValid, exprNode] = expression();
            if(!exprValid){
                throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
                    + " -> SYNTAX ERROR near: " + currentToken.value);
            }
            tempNode->pushChild(exprNode);
        }
        return currentNode;
    }
    else{
        throw std::runtime_error("Line " + std::to_string(currentToken.line) + ", Column " + std::to_string(currentToken.column) 
            + " -> SYNTAX ERROR near: " + currentToken.value);
    }
}

std::pair<bool, std::shared_ptr<ASTree>> Parser::expression(){
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::EXPRESSION, Token());
    if(currentToken.type == TokenType::_LITERAL){
        scopeManager.pushSymbol(Symbol(currentToken.value, Kinds::LIT, Types::INT));
        currentNode->pushChild(std::make_shared<ASTree>(ASTNodeType::LITERAL, currentToken));

        eat(TokenType::_LITERAL);
        return {true, currentNode};
    }
    else if(currentToken.type == TokenType::_ID && lexer.peekAtNext().type == TokenType::_LPAREN){
        currentNode->pushChild(functionCall());
        return {true, currentNode};
    }
    else if(currentToken.type == TokenType::_ID){
        currentNode->pushChild(std::make_shared<ASTree>(ASTNodeType::ID, currentToken));
        eat(TokenType::_ID);
        return {true, currentNode};
    }
    else if(currentToken.type == TokenType::_LPAREN){
        eat(TokenType::_LPAREN);
        currentNode->pushChild(numericalExpression());
        eat(TokenType::_RPAREN);
        return {true, currentNode};
    }
    return {false, nullptr};
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
    auto currentNode = std::make_shared<ASTree>(ASTNodeType::ARGUMENT, Token());
    while(currentToken.type != TokenType::_RPAREN){
        currentNode->pushChild(numericalExpression());
        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().type != TokenType::_RPAREN){
            eat(TokenType::_COMMA);
        }
    }
    return currentNode;
}