#include "../include/parser.hpp"

Parser::Parser(Lexer& lexer) : lexer(lexer), currentToken(lexer.nextToken()) {}

void Parser::parseProgram(){
    functionList();
    if(currentToken.type != TokenType::_EOF){
        throw std::runtime_error("Line x: SYNTAX ERROR near: " + currentToken.value);
    }
}

void Parser::eat(TokenType type){
    if(currentToken.type == type){
        currentToken = lexer.nextToken();
    } 
    else{
        throw std::runtime_error("Line x: SYNTAX ERROR near: " + currentToken.value);
    }
}

void Parser::functionList(){
    while(currentToken.type == TokenType::_TYPE){
        function();
    }
}

void Parser::function(){
    eat(TokenType::_TYPE);
    eat(TokenType::_ID);
    eat(TokenType::_LPAREN);
    parameter();
    eat(TokenType::_RPAREN);
    body();
}

void Parser::parameter(){
    while(currentToken.type == TokenType::_TYPE){
        eat(TokenType::_TYPE);  
        eat(TokenType::_ID);
        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().type == TokenType::_TYPE){
            eat(TokenType::_COMMA);
        }
    }
}

void Parser::body(){
    eat(TokenType::_LBRACKET);
    variableList();
    statementList();
    eat(TokenType::_RBRACKET);
}

void Parser::variableList(){
    while(currentToken.type == TokenType::_TYPE){
        variable();
    }
}

void Parser::variable(){
    eat(TokenType::_TYPE);
    eat(TokenType::_ID);
    eat(TokenType::_SEMICOLON);
}

void Parser::statementList(){
    while(currentToken.type != TokenType::_RBRACKET){
        statement();
    }
}

void Parser::statement(){
    if(lexer.peekAtNext().type == TokenType::_ASSIGN){
        assignmentStatement();
    }
    else if(currentToken.type == TokenType::_RETURN){
        returnStatement();
    }
    else if(currentToken.type == TokenType::_IF){
        ifStatement();
    }
    else if(currentToken.type == TokenType::_LBRACKET){
        compoundStatement();
    }
    else{
        throw std::runtime_error("Line x: SYNTAX ERROR near: " + currentToken.value);
    }
}

void Parser::compoundStatement(){
    eat(TokenType::_LBRACKET);
    statementList();
    eat(TokenType::_RBRACKET);
}

void Parser::assignmentStatement(){
    if(currentToken.type == TokenType::_ID){
        eat(TokenType::_ID);      
        eat(TokenType::_ASSIGN);
        numericalExpression();
        eat(TokenType::_SEMICOLON);
    }
    else{
        throw std::runtime_error("Line x: SYNTAX ERROR near: " + currentToken.value);
    }
}

void Parser::returnStatement(){
    if(currentToken.type == TokenType::_RETURN){
        eat(TokenType::_RETURN);
        if(currentToken.type == TokenType::_SEMICOLON){
            eat(TokenType::_SEMICOLON);
        }
        else{
            numericalExpression();
            eat(TokenType::_SEMICOLON);
        }
    }
    else{
        throw std::runtime_error("Line x: SYNTAX ERROR near: " + currentToken.value);
    }
}

void Parser::ifStatement(){
    eat(TokenType::_IF);
    eat(TokenType::_LPAREN);
    relationalExpression();
    eat(TokenType::_RPAREN);
    statement();
    if(currentToken.type == TokenType::_ELSE){
        eat(TokenType::_ELSE);
        statement();
    }
}

void Parser::numericalExpression(){
    if(expression()){
        while(currentToken.type == TokenType::_AROP){
            eat(TokenType::_AROP);
            if(!expression()){
                throw std::runtime_error("Line x: SYNTAX ERROR near: " + currentToken.value);
            }
        }
    }
    else{
        throw std::runtime_error("Line x: SYNTAX ERROR near: " + currentToken.value);
    }
}

bool Parser::expression(){
    if(currentToken.type == TokenType::_LITERAL){
        eat(TokenType::_LITERAL);
        return true;
    }
    else if(currentToken.type == TokenType::_ID && lexer.peekAtNext().type == TokenType::_LPAREN){
        functionCall();
        return true;
    }
    else if(currentToken.type == TokenType::_ID){
        eat(TokenType::_ID);
        return true;
    }
    else if(currentToken.type == TokenType::_LPAREN){
        eat(TokenType::_LPAREN);
        numericalExpression();
        eat(TokenType::_RPAREN);
        return true;
    }
    return false;
}

void Parser::relationalExpression(){
    numericalExpression();
    eat(TokenType::_RELOP);
    numericalExpression();
}

void Parser::functionCall(){
    eat(TokenType::_ID);
    eat(TokenType::_LPAREN);
    argument();
    eat(TokenType::_RPAREN);
}

void Parser::argument(){
    while(currentToken.type != TokenType::_RPAREN){
        numericalExpression();
        if(currentToken.type == TokenType::_COMMA && lexer.peekAtNext().type != TokenType::_RPAREN){
            eat(TokenType::_COMMA);
        }
    }
}