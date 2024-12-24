#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"
#include "scope_manager.hpp"

class Parser {
public:
    Parser(Lexer& lexer, ScopeManager& scopeManager);
    
    void parseProgram();

private:
    Lexer& lexer;
    ScopeManager& scopeManager;
    Token currentToken;

    void eat(TokenType type);

    void functionList();

    void function();

    void parameter();

    void body();

    void variableList();

    void variable();

    void statementList();

    void statement();

    void compoundStatement();

    void assignmentStatement();

    void returnStatement();

    void ifStatement();

    void numericalExpression();

    bool expression();

    void relationalExpression();

    void functionCall();

    void argument();

};

#endif