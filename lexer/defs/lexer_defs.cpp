#include "lexer_defs.hpp"

const std::unordered_map<std::string, TokenType> keywords {
    {"return", TokenType::_RETURN},
    {"if", TokenType::_IF},
    {"else", TokenType::_ELSE},
    {"int", TokenType::_INT},
    {"unsigned", TokenType::_UNSIGNED},
    {"void", TokenType::_VOID},
    {"auto", TokenType::_AUTO},
    {"while", TokenType::_WHILE},
    {"for", TokenType::_FOR},
    {"do", TokenType::_DO},
    {"switch", TokenType::_SWITCH},
    {"case", TokenType::_CASE},
    {"default", TokenType::_DEFAULT},
    {"break", TokenType::_BREAK},
    {"printf", TokenType::_PRINTF}
};

const std::unordered_set<TokenType> types {
    TokenType::_INT,
    TokenType::_UNSIGNED,
    TokenType::_AUTO,
    TokenType::_VOID
};