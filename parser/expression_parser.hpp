#ifndef EXPRESSION_PARSER_HPP
#define EXPRESSION_PARSER_HPP

#include <memory>

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
    std::unique_ptr<AST::node::ASTExpr> parseExpr();

    /** 
     * @brief parses id expression
     * @details ID : ID
     * @returns pointer to an id node
    */
    std::unique_ptr<AST::node::ASTIdExpr> parseIdExpr();

    /** 
     * @brief parses literal expression
     * @details LITERAL : LITERAL(unsigned) | LITERAL(int)
     * @returns pointer to a literal node
    */
    std::unique_ptr<AST::node::ASTLiteralExpr> parseLiteralExpr();

    /** 
     * @brief parses function call
     * @details FUNCTION_CALL : ID LPAREN (ARGUMENT)? RPAREN
     * @returns pointer to a function call node
    */
    std::unique_ptr<AST::node::ASTFunctionCallExpr> parseFunctionCallExpr();

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
    std::unique_ptr<AST::node::ASTExpr> parsePrimaryExpr();

    /** 
     * @brief parses argument of the function call
     * @param callExpr - function call that owns arguments
     * @details (EXPRESSION (COMMA EXPRESSION)*)?
    */
    void parseArguments(AST::node::ASTFunctionCallExpr* callExpr);

    /** 
     * @brief parses the root of the binary expression node (only operator)
     * @returns pointer to a binary expression node
    */
    std::unique_ptr<AST::node::ASTBinaryExpr> parseOperator();

private:
    /// reference to a token handler wrapped around the lexer
    TokenConsumer& tokenConsumer;

};

#endif
