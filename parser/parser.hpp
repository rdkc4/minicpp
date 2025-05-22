#ifndef PARSER_HPP
#define PARSER_HPP

#include <stack>

#include "../lexer/lexer.hpp"
#include "../common/abstract-syntax-tree/ASTree.hpp"

class Parser{
    public:

        Parser(Lexer& lexer);
        
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // public entry point for parsing
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::unique_ptr<ASTree> parseProgram();

    private:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // lexer - token handler
        // current token - currently parsed token
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        Lexer& lexer;
        Token currentToken;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // token parsing
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void eat(TokenType type);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // functions
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::unique_ptr<ASTree> functionList();

        std::unique_ptr<ASTree> function();

        std::unique_ptr<ASTree> parameter();

        std::unique_ptr<ASTree> body();

        std::unique_ptr<ASTree> construct();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // variables
        //-----------------------------------------------------------------------------------------------------------------------------------------------------

        std::unique_ptr<ASTree> variable();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // statements
        //-----------------------------------------------------------------------------------------------------------------------------------------------------

        std::unique_ptr<ASTree> statement();

        std::unique_ptr<ASTree> compoundStatement();

        std::unique_ptr<ASTree> assignmentStatement();

        std::unique_ptr<ASTree> returnStatement();

        std::unique_ptr<ASTree> ifStatement();

        std::unique_ptr<ASTree> whileStatement();

        std::unique_ptr<ASTree> forStatement();

        std::unique_ptr<ASTree> doWhileStatement();

        std::unique_ptr<ASTree> switchStatement();

        std::unique_ptr<ASTree> switchCaseConstruct();

        std::unique_ptr<ASTree> _case();

        std::unique_ptr<ASTree> _default();

        std::unique_ptr<ASTree> _break();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // expressions
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::unique_ptr<ASTree> numericalExpression();

        int getPrecedence(const std::string& op);

        std::unique_ptr<ASTree> rpnToTree(std::stack<std::unique_ptr<ASTree>>& st, std::unique_ptr<ASTree>& parent);

        std::unique_ptr<ASTree> expression();

        std::unique_ptr<ASTree> relationalExpression();

        std::unique_ptr<ASTree> functionCall();

        std::unique_ptr<ASTree> argument();

        std::unique_ptr<ASTree> id();

        std::unique_ptr<ASTree> literal();

};

#endif
