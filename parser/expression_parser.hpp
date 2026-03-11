#ifndef EXPRESSION_PARSER_HPP
#define EXPRESSION_PARSER_HPP

#include <memory>
#include <stack>
#include <unordered_map>

#include "../common/abstract-syntax-tree/ast_expr.hpp"
#include "../common/abstract-syntax-tree/ast_function_call_expr.hpp"
#include "../common/abstract-syntax-tree/ast_literal_expr.hpp"
#include "../common/abstract-syntax-tree/ast_id_expr.hpp"
#include "../common/abstract-syntax-tree/ast_binary_expr.hpp"

#include "token_consumer.hpp"

/** 
 * @class ExpressionParser
 * @brief used for parsing expressions
*/
class ExpressionParser {
public:
    /** 
     * @brief Creates new instance of the parser specialized for expressions
     * @param consumer - reference to token handler wrapped around the lexer 
    */
    ExpressionParser(TokenConsumer& consumer);

    /** 
     * @brief parses numerical expression
     * @details NUMERICAL_EXPRESSION : EXPRESSION (OPERATOR EXPRESSION)*
     * @returns pointer to an expression node
    */
    std::unique_ptr<ASTExpr> parseNumericalExpr();

    /** 
     * @brief parses relational expression
     * @details RELATIONAL EXPRESSION : NUMERICAL_EXPRESSION RELOP NUMERICAL_EXPRESSION
     * @returns pointer to an expression node
    */
    std::unique_ptr<ASTExpr> parseRelationalExpr();

    /** 
     * @brief parses id expression
     * @details ID : ID
     * @returns pointer to an id node
    */
    std::unique_ptr<ASTIdExpr> parseIdExpr();

    /** 
     * @brief parses literal expression
     * @details LITERAL : LITERAL(unsigned) | LITERAL(int)
     * @returns pointer to a literal node
    */
    std::unique_ptr<ASTLiteralExpr> parseLiteralExpr();

    /** 
     * @brief parses function call
     * @details FUNCTION_CALL : ID LPAREN (ARGUMENT)? RPAREN
     * @returns pointer to a function call node
    */
    std::unique_ptr<ASTFunctionCallExpr> parseFunctionCallExpr();

    /** 
     * @brief transforms rpn stack to a tree
     * @param rpn - reference to a rpn stack
     * @param root - pointer to the root of the (sub)expression 
     * @returns pointer to the transformed expression node
    */
    std::unique_ptr<ASTExpr> rpnToTree(std::stack<std::unique_ptr<ASTExpr>>& rpn, std::unique_ptr<ASTExpr>& root) const;

    /** 
     * @brief parses expression
     * @details
     *
     * EXPRESSION
     * 
     * : LITERAL
     *
     * | FUNCTION_CALL
     *
     * | ID
     *
     * | LPAREN NUMERICAL_EXPRESSION RPAREN
     * @returns pointer to an expression node
    */
    std::unique_ptr<ASTExpr> parseExpr();

    /** 
     * @brief parses argument of the function call
     * @param callExpr - function call that owns arguments
     * @details (NUMERICAL_EXPRESSION (COMMA NUMERICAL_EXPRESSION)*)?
    */
    void parseArguments(ASTFunctionCallExpr* callExpr);

    /** 
     * @brief parses the root of the binary expression node (only operator)
     * @param isRel - flag if operator is relational or not
     * @returns pointer to a binary expression node
    */
    std::unique_ptr<ASTBinaryExpr> parseOperator(bool isRel = false);

    /** 
     * @brief calculates the precedence of the operator
     * @note higher value means stronger precedence
     * @param op - operator
     * @returns strength of the precedence of the operator
    */
    int getPrecedence(const std::string& op) const noexcept {
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

private:
    /// reference to a token handler wrapped around the lexer
    TokenConsumer& tokenConsumer;

};

#endif
