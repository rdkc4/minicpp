#ifndef DEFS_HPP
#define DEFS_HPP

#include <iostream>

enum class TokenType{_ID, _LITERAL, _AROP, _RELOP, _LPAREN, _RPAREN, _LBRACKET, _RBRACKET, _SEMICOLON,
    _ASSIGN, _TYPE, _IF, _ELSE, _RETURN, _COMMA, _EOF, _INVALID
};

enum class Types{INT, UNSIGNED, VOID, NO_TYPE};

enum class Kinds{NO_KIND = 0x1, REG = 0x2, LIT = 0x4, FUN = 0x8, VAR = 0x10, PAR = 0x20};

enum class ArithmeticOperators{ADD, SUB, MUL, DIV, AROP_NUMBER};

enum class RelationalOperator{LT, GT, LE, GE, EQ, NE, RELOP_NUMBER};

struct Token{
    TokenType type;
    std::string value;

    Token(TokenType type, const std::string& value): type(type), value(value){}
};

#endif
