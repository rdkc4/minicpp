#ifndef EXPRESSION_PARSER_HPP
#define EXPRESSION_PARSER_HPP

#include <memory>
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
     * @brief parses arithmetic expression
     * @details EXPRESSION : EXPRESSION (OPERATOR EXPRESSION)*
     * @returns pointer to an expression node
    */
    std::unique_ptr<ASTExpr> parseExpr();

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
     * @brief parses expression
     * @details
     *
     * PRIMARY_EXPRESSION
     * 
     * : LITERAL
     *
     * | FUNCTION_CALL
     *
     * | ID
     *
     * | LPAREN EXPRESSION RPAREN
     * @returns pointer to an expression node
    */
    std::unique_ptr<ASTExpr> parsePrimaryExpr();

    /** 
     * @brief parses argument of the function call
     * @param callExpr - function call that owns arguments
     * @details (EXPRESSION (COMMA EXPRESSION)*)?
    */
    void parseArguments(ASTFunctionCallExpr* callExpr);

    /** 
     * @brief parses the root of the binary expression node (only operator)
     * @returns pointer to a binary expression node
    */
    std::unique_ptr<ASTBinaryExpr> parseOperator();

    /** 
     * @brief calculates the precedence of the operator
     * @note higher value means stronger precedence
     * @param type - type of the operator
     * @returns strength of the precedence of the operator
    */
    int getPrecedence(TokenType type) const noexcept {
        const static std::unordered_map<TokenType, int> precedence {
            {TokenType::LOGICAL_OR, 1},
            {TokenType::LOGICAL_AND, 2},
            {TokenType::PIPE, 3},
            {TokenType::CARET, 4},
            {TokenType::AMPERSEND, 5},
            {TokenType::EQUAL, 6},
            {TokenType::NOT_EQ, 6},
            {TokenType::GREATER, 7},
            {TokenType::LESS, 7},
            {TokenType::GREATER_EQ, 7},
            {TokenType::LESS_EQ, 7},
            {TokenType::LSHIFT, 8},
            {TokenType::RSHIFT, 8},
            {TokenType::MINUS, 9},
            {TokenType::PLUS, 9},
            {TokenType::STAR, 10},
            {TokenType::SLASH, 10}
        };

        auto it{ precedence.find(type) };
        return it != precedence.end() ? it->second : 0;
    }

private:
    /// reference to a token handler wrapped around the lexer
    TokenConsumer& tokenConsumer;

};

#endif
