#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

#include "defs/token_defs.hpp"

/** 
 * @struct Token
 * @brief description of a token
*/
struct Token{
    /// literal value of the token
    std::string value;
    /// line where token was found
    size_t line;
    /// column where token ends
    size_t column;
    /// type of a token
    TokenType type;
    /// general type of a token
    GeneralTokenType gtype;

    /** 
     * @brief Creates a new instance of a token
    */
    Token();

    /** 
     * @brief Creates a new instance of a token
     * @param value - literal value of the token
     * @param line - line where token was found
     * @param column - column where token ends
    */
    Token(std::string_view value, size_t line, size_t column);

    /** 
     * @brief Creates a new instance of a token
     * @param value - literal value of the token
     * @param line - line where token was found
     * @param column - column where token ends
     * @param type - type of the token
    */
    Token(std::string_view value, size_t line, size_t column, TokenType type);

    /** 
     * @brief Creates a new instance of a token
     * @param value - literal value of the token
     * @param line - line where token was found
     * @param column - column where token ends
     * @param type - type of the token
     * @param gtype - general type of the token
    */
    Token(std::string_view value, size_t line, size_t column, TokenType type, GeneralTokenType gtype);
};

#endif
