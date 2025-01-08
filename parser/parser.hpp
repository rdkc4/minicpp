#ifndef PARSER_HPP
#define PARSER_HPP

#include <stack>

#include "../lexer/lexer.hpp"
#include "../common/abstract-syntax-tree/ASTree.hpp"

class Parser{
    public:
        Parser(Lexer& lexer);
        
        std::shared_ptr<ASTree> parseProgram();

    private:
        Lexer& lexer;
        Token currentToken;

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

        int getPrecedence(std::string& op);

        std::shared_ptr<ASTree> rpnToTree(std::stack<std::shared_ptr<ASTree>>& st, std::shared_ptr<ASTree> parent);

        std::shared_ptr<ASTree> expression();

        std::shared_ptr<ASTree> relationalExpression();

        std::shared_ptr<ASTree> functionCall();

        std::shared_ptr<ASTree> argument();

        std::shared_ptr<ASTree> id();

        std::shared_ptr<ASTree> literal();

};

#endif