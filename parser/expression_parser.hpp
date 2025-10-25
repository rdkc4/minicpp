#ifndef EXPRESSION_PARSER_HPP
#define EXPRESSION_PARSER_HPP

#include <memory>
#include <stack>
#include <unordered_map>

#include "../common/abstract-syntax-tree/ASTExpression.hpp"
#include "../common/abstract-syntax-tree/ASTFunctionCall.hpp"
#include "../common/abstract-syntax-tree/ASTLiteral.hpp"
#include "../common/abstract-syntax-tree/ASTId.hpp"
#include "../common/abstract-syntax-tree/ASTBinaryExpression.hpp"

#include "token_consumer.hpp"

class ExpressionParser {
public:
    ExpressionParser(TokenConsumer& consumer);

    std::unique_ptr<ASTExpression> numericalExpression();
    std::unique_ptr<ASTExpression> relationalExpression();
    std::unique_ptr<ASTId> id();
    std::unique_ptr<ASTLiteral> literal();

protected:
    TokenConsumer& tokenConsumer;

    // expressions
    std::unique_ptr<ASTExpression> rpnToTree(std::stack<std::unique_ptr<ASTExpression>>& rpn, std::unique_ptr<ASTExpression>& root) const;
    std::unique_ptr<ASTExpression> expression();
    std::unique_ptr<ASTFunctionCall> functionCall();
    void argument(ASTFunctionCall* _functionCall);
    std::unique_ptr<ASTBinaryExpression> _operator(bool isRel = false);

    // precedence calculator
    // higher value = stronger precedence
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