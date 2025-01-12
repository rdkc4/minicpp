#ifndef LEXER_DEFS_HPP
#define LEXER_DEFS_HPP

#include <unordered_map>
#include <string>

#include "../../common/token/token.hpp"

extern const std::unordered_map<std::string, TokenType> keywords;

#endif