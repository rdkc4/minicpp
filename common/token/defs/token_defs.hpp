#ifndef TOKEN_DEFS_HPP
#define TOKEN_DEFS_HPP

#include <string>
#include <unordered_map>

/*
    token related definitions
*/

enum class TokenType{
    _ID, _LITERAL, _AROP, _BITWISE, _RELOP, _LPAREN, _RPAREN, _LBRACKET, _RBRACKET, _SEMICOLON, _COLON, _ASSIGN,
    _IF, _ELSE, _WHILE, _DO, _FOR, _SWITCH, _CASE, _DEFAULT, _BREAK, _RETURN, _COMMA, _EOF, _INVALID, _NODE, _PRINTF,
    _INT, _UNSIGNED, _VOID, _AUTO
};

enum class GeneralTokenType { VALUE, OPERATOR, TYPE, OTHER };

extern const std::unordered_map<TokenType, std::string> tokenTypeToString;

extern const std::unordered_map<GeneralTokenType, std::string> generalTokenTypeToString;

#endif