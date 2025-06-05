#ifndef LEXER_DEFS_HPP
#define LEXER_DEFS_HPP

#include <unordered_map>
#include <string>

#include "../../common/token/token.hpp"

/*
    lexer related definitions
*/

// keywords reserved for the language
extern const std::unordered_map<std::string, TokenType> keywords;

#endif
