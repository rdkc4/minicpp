#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <unordered_map>

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// definitions related to tokens
//-----------------------------------------------------------------------------------------------------------------------------------------------------

enum class TokenType{
    _ID, _LITERAL, _AROP, _BITWISE, _RELOP, _LPAREN, _RPAREN, _LBRACKET, _RBRACKET, _SEMICOLON, _COLON, _ASSIGN, _TYPE,
    _IF, _ELSE, _WHILE, _DO, _FOR, _SWITCH, _CASE, _DEFAULT, _BREAK, _RETURN, _COMMA, _EOF, _INVALID, _NODE
};

extern const std::unordered_map<TokenType, std::string> tokenTypeToString;

struct Token{

    //-----------------------------------------------------------------------------------------------------------------------------------------------------
    // value - more specific description
    // line - line where token was found
    // column - column where token begins
    // type - type of a token
    //-----------------------------------------------------------------------------------------------------------------------------------------------------
    std::string value;
    size_t line;
    size_t column;
    TokenType type;

    Token();
    Token(std::string_view value, size_t line, size_t column);
    Token(std::string_view value, size_t line, size_t column, TokenType type);
};

#endif
