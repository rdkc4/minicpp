#ifndef FUNCTION_PARSER_HPP
#define FUNCTION_PARSER_HPP

#include <memory>

#include "../common/abstract-syntax-tree/ast_function.hpp"
#include "statement_parser.hpp"
#include "token_consumer.hpp"

/** 
 * @class FunctionParser
 * @brief used for parsing functions
*/
class FunctionParser {
public:
    /** 
     * @brief Creates new instance of the parser specialized for functions
     * @param consumer - reference to token handler wrapped around the lexer 
    */
    FunctionParser(TokenConsumer& consumer);

    /** 
     * @brief parses function
     * @details FUNCTION : TYPE ID LPAREN (PARAMETER (COMMA PARAMETER)*)? RPAREN BODY
     * @returns pointer to a function node
    */
    std::unique_ptr<ASTFunction> parseFunction();

    /** 
     * @brief parses parameters of the function
     * @param function - pointer to a function that owns parameters
     * @details PARAMETER : (TYPE ID (COMMA TYPE ID)*)?
    */
    void parseParameters(ASTFunction* function);

    /** 
     * @brief parses body of the function
     * @param function - pointer to a function that owns the body
     * @details BODY : LBRACKET (STATEMENT)? RBRACKET
    */
    void parseBody(ASTFunction* function);

private:
    /// parser specialized for statements
    StatementParser stmtParser;

    /// reference to a token handler wrapped around the lexer
    TokenConsumer& tokenConsumer;

};

#endif