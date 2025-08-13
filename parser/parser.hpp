#ifndef PARSER_HPP
#define PARSER_HPP

#include <stack>
#include <unordered_map>
#include <string>

#include "../lexer/lexer.hpp"
#include "../common/abstract-syntax-tree/ASTProgram.hpp"
#include "../common/abstract-syntax-tree/ASTFunction.hpp"
#include "../common/abstract-syntax-tree/ASTStatement.hpp"
#include "../common/abstract-syntax-tree/ASTVariable.hpp"
#include "../common/abstract-syntax-tree/ASTPrintfSt.hpp"
#include "../common/abstract-syntax-tree/ASTCompoundSt.hpp"
#include "../common/abstract-syntax-tree/ASTAssignSt.hpp"
#include "../common/abstract-syntax-tree/ASTReturnSt.hpp"
#include "../common/abstract-syntax-tree/ASTIfSt.hpp"
#include "../common/abstract-syntax-tree/ASTWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTDoWhileSt.hpp"
#include "../common/abstract-syntax-tree/ASTForSt.hpp"
#include "../common/abstract-syntax-tree/ASTSwitchSt.hpp"
#include "../common/abstract-syntax-tree/ASTExpression.hpp"
#include "../common/abstract-syntax-tree/ASTFunctionCall.hpp"
#include "../common/abstract-syntax-tree/ASTLiteral.hpp"
#include "../common/abstract-syntax-tree/ASTId.hpp"
#include "../common/abstract-syntax-tree/ASTBinaryExpression.hpp"

/*
    Syntax analysis of the tokenized input
    Abstract syntax tree generation
*/
class Parser{
    public:

        Parser(Lexer& lexer);
        
        // parsing of the tokenized input
        // returns the root of the abstract syntax tree
        [[nodiscard]] std::unique_ptr<ASTProgram> parseProgram();

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
        void functionList(ASTProgram* _program);
        std::unique_ptr<ASTFunction> function();
        void parameter(ASTFunction* _function);
        void body(ASTFunction* _function);

        // statements
        std::unique_ptr<ASTStatement> statement();
        std::unique_ptr<ASTVariable> variable();
        std::unique_ptr<ASTPrintfSt> printfStatement();
        std::unique_ptr<ASTCompoundSt> compoundStatement();
        std::unique_ptr<ASTAssignSt> assignmentStatement();
        std::unique_ptr<ASTReturnSt> returnStatement();
        std::unique_ptr<ASTIfSt> ifStatement();
        std::unique_ptr<ASTWhileSt> whileStatement();
        std::unique_ptr<ASTForSt> forStatement();
        std::unique_ptr<ASTDoWhileSt> doWhileStatement();
        std::unique_ptr<ASTSwitchSt> switchStatement();
        std::unique_ptr<ASTSwitchBlock> switchCaseBlock();
        std::unique_ptr<ASTCaseSt> _case();
        std::unique_ptr<ASTDefaultSt> _default();
        void _break();

        // expressions
        std::unique_ptr<ASTExpression> numericalExpression();
        std::unique_ptr<ASTExpression> rpnToTree(std::stack<std::unique_ptr<ASTExpression>>& rpn, std::unique_ptr<ASTExpression>& root) const;
        std::unique_ptr<ASTExpression> expression();
        std::unique_ptr<ASTExpression> relationalExpression();
        std::unique_ptr<ASTFunctionCall> functionCall();
        void argument(ASTFunctionCall* _functionCall);
        std::unique_ptr<ASTId> id();
        std::unique_ptr<ASTLiteral> literal();
        std::unique_ptr<ASTBinaryExpression> _operator(bool isRel = false);

        // precedence calculator
        constexpr int getPrecedence(const std::string& op) const noexcept {
            const static std::unordered_map<std::string, int> precedence {
                {"|", 1},
                {"^", 2},
                {"&", 3},
                {"==", 4},
                {"!=", 4},
                {">", 5},
                {"<", 5},
                {">=", 5},
                {"<=", 5},
                {"<<", 6},
                {">>", 6},
                {"-", 7},
                {"+", 7},
                {"*", 8},
                {"/", 8}
            };

            auto it = precedence.find(op);
            return it != precedence.end() ? it->second : 0;
        }

};

#endif
