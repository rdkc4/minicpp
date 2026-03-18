#ifndef TOKEN_DEFS_HPP
#define TOKEN_DEFS_HPP

#include <string>
#include <unordered_map>

/** 
 * @enum TokenType
 * @brief types of the tokens defined by the compiler
*/
enum class TokenType{
    _ID, _LITERAL, 
    _AROP, _BITWISE, _RELOP, _ASSIGN,
    _LPAREN, _RPAREN, _LBRACKET, _RBRACKET, 
    _SEMICOLON, _COLON, _HASH, _COMMA,
    _IF, _ELSE, _WHILE, _DO, _FOR, _RETURN,
    _SWITCH, _CASE, _DEFAULT, _BREAK,   
    _EOF, _INVALID, _NODE,
    _INT, _UNSIGNED, _VOID, _AUTO, 
    _INCLUDE
};

/** 
 * @enum GeneralTokenType
 * @brief general types of the tokens
*/
enum class GeneralTokenType { VALUE, OPERATOR, TYPE, OTHER };

/// maps token type to a string
extern const std::unordered_map<TokenType, std::string> tokenTypeToString;

/// maps general token type to a string
extern const std::unordered_map<GeneralTokenType, std::string> generalTokenTypeToString;

#endif