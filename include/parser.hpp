#ifndef PARSER_HPP
#define PARSER_HPP

#include <stack>

#include "lexer.hpp"
#include "scope_manager.hpp"
#include "ASTree.hpp"
#include "analyzer.hpp"
#include "intermediate_representation.hpp"
#include "code_generator.hpp"

class Parser{
    public:
        Parser(Lexer& lexer, ScopeManager& scopeManager);
        
        void parseProgram();

    private:
        Lexer& lexer;
        Token currentToken;
        Analyzer analyzer;

        std::shared_ptr<ASTree> root;

        IntermediateRepresentation intermediateRepresentation;

        CodeGenerator codeGen;

        Types getTypeFromToken(Token token);

        void eat(TokenType type);

        std::shared_ptr<ASTree> functionList();

        std::shared_ptr<ASTree> function();

        std::shared_ptr<ASTree> parameter();

        std::shared_ptr<ASTree> body();

        std::shared_ptr<ASTree> variableList();

        std::shared_ptr<ASTree> variable();

        std::shared_ptr<ASTree> statementList();

        std::shared_ptr<ASTree> statement();

        std::shared_ptr<ASTree> compoundStatement();

        std::shared_ptr<ASTree> assignmentStatement();

        std::shared_ptr<ASTree> returnStatement();

        std::shared_ptr<ASTree> ifStatement();

        std::shared_ptr<ASTree> whileStatement();

        std::shared_ptr<ASTree> forStatement();

        std::shared_ptr<ASTree> doWhileStatement();

        std::shared_ptr<ASTree> switchStatement();

        std::shared_ptr<ASTree> switchCaseStatementList();

        std::shared_ptr<ASTree> _case();

        std::shared_ptr<ASTree> _default();

        std::shared_ptr<ASTree> _break();

        std::shared_ptr<ASTree> numericalExpression();

        std::shared_ptr<ASTree> rpnToTree(std::stack<std::shared_ptr<ASTree>>& st, std::shared_ptr<ASTree> parent);

        std::shared_ptr<ASTree> expression();

        std::shared_ptr<ASTree> relationalExpression();

        std::shared_ptr<ASTree> functionCall();

        std::shared_ptr<ASTree> argument();

        std::shared_ptr<ASTree> id();

        std::shared_ptr<ASTree> literal();

};

#endif