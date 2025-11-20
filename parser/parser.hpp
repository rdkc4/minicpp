#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>

#include "../common/abstract-syntax-tree/ASTProgram.hpp"

#include "function_parser.hpp"
#include "token_consumer.hpp"

/**
 * @class Parser
 * @brief syntax analysis of the tokenized input
 *
 * generates abstract syntax tree
*/
class Parser {
public:
    /** 
     * @brief Creates new instance of the parser
     * @param consumer - reference to token handler wrapped around the lexer 
    */
    Parser(TokenConsumer& consumer);
        
    /** 
     * @brief entry point for the parsing of the program
     * @details PROGRAM : (FUNCTION)+
     * @returns pointer to the root of the ast of the program
    */
    [[nodiscard]] std::unique_ptr<ASTProgram> parseProgram();

private:
    /// parser specialized for functions
    FunctionParser funcParser;
    /// reference to a token handler wrapped around the lexer
    TokenConsumer& tokenConsumer;

};

#endif

/* TODO:
 * GLOBAL VARIABLES
 * RELATIONAL EXPRESSIONS WITH MULTIPLE CONDITIONS
 * MORE TYPES
 * ENUMERATORS
 * LISTS
 * ANONYMOUS FUNCTIONS
 * INCLUDE / HANDLING MULTIPLE FILES
 * PRE/POST INCREMENT/DECREMENT
 * BITWISE OPERATORS (!, ~)
 * TERNARY OPERATOR ? :
*/