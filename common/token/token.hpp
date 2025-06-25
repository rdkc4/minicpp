#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

#include "defs/token_defs.hpp"

struct Token{
    // more specific description
    std::string value;
    // line where token was found
    size_t line;
    // column where token was found
    size_t column;
    // type of a token
    TokenType type;
    // general type of a token
    GeneralTokenType gtype;

    Token();
    Token(std::string_view value, size_t line, size_t column);
    Token(std::string_view value, size_t line, size_t column, TokenType type);
    Token(std::string_view value, size_t line, size_t column, TokenType type, GeneralTokenType gtype);
};

#endif
