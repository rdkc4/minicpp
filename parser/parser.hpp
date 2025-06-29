#ifndef PARSER_HPP
#define PARSER_HPP

#include <stack>

#include "../lexer/lexer.hpp"
#include "../common/abstract-syntax-tree/ASTree.hpp"

/*
    Syntax analysis of the tokenized input
    Abstract syntax tree generation
*/
class Parser{
    public:

        Parser(Lexer& lexer);
        
        // parsing of the tokenized input
        // returns the root of the abstract syntax tree
        [[nodiscard]] std::unique_ptr<ASTree> parseProgram();

    protected:
        // token handler
        Lexer& lexer;
        
        // currently parsed token
        Token currentToken;

        // token checking
        // throws if current token's type differs from expected type
        void consume(TokenType expectedType);
        void consume(GeneralTokenType expectedGType);

        // functions
        std::unique_ptr<ASTree> functionList();
        std::unique_ptr<ASTree> function();
        std::unique_ptr<ASTree> parameter();
        std::unique_ptr<ASTree> body();
        
        std::unique_ptr<ASTree> construct();

        std::unique_ptr<ASTree> variable();

        // statements
        std::unique_ptr<ASTree> statement();
        std::unique_ptr<ASTree> printfStatement();
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

        // expressions
        std::unique_ptr<ASTree> numericalExpression();
        std::unique_ptr<ASTree> rpnToTree(std::stack<std::unique_ptr<ASTree>>& rpn, std::unique_ptr<ASTree>& root) const;
        std::unique_ptr<ASTree> expression();
        std::unique_ptr<ASTree> relationalExpression();
        std::unique_ptr<ASTree> functionCall();
        std::unique_ptr<ASTree> argument();
        std::unique_ptr<ASTree> id();
        std::unique_ptr<ASTree> literal() const;

        // precedence calculator
        constexpr int getPrecedence(std::string_view op) const noexcept {
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
            return 0;
        }

};

#endif
