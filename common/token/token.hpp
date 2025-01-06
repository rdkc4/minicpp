#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <unordered_map>

enum class TokenType{
    _ID, _LITERAL, _AROP, _BITWISE, _RELOP, _LPAREN, _RPAREN, _LBRACKET, _RBRACKET, _SEMICOLON, _COLON, _ASSIGN, _TYPE,
    _IF, _ELSE, _WHILE, _DO, _FOR, _SWITCH, _CASE, _DEFAULT, _BREAK, _RETURN, _COMMA, _EOF, _INVALID, _NODE
};

extern const std::unordered_map<TokenType, std::string> tokenTypeToString;

struct Token{
    TokenType type;
    std::string value;
    size_t line;
    size_t column;

    Token() : type(TokenType::_NODE), value(""), line(0), column(0){}
    Token(std::string value, size_t line, size_t column) : type(TokenType::_NODE), value(value), line(line), column(column){}
    Token(TokenType type, const std::string& value, size_t line, size_t column): type(type), value(value), line(line), column(column){}
};

#endif