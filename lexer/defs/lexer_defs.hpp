#ifndef LEXER_DEFS_HPP
#define LEXER_DEFS_HPP

#include <unordered_map>
#include <string>
#include <unordered_set>

#include "../../common/token/defs/token_defs.hpp"

/// keywords reserved for the compiler
extern const std::unordered_map<std::string, TokenType> keywords;

/// types reserved for the compiler
extern const std::unordered_set<TokenType> types;

#endif
