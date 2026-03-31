#include "lexer_defs.hpp"

const std::unordered_map<std::string_view, TokenType> keywords {
    /// preprocessor / directive keywords
    {"include", TokenType::INCLUDE},

    /// control flow keywords
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"return", TokenType::RETURN},
    {"while", TokenType::WHILE},
    {"for", TokenType::FOR},
    {"do", TokenType::DO},
    {"switch", TokenType::SWITCH},
    {"case", TokenType::CASE},
    {"default", TokenType::DEFAULT},
    {"break", TokenType::BREAK},

    /// type keywords
    {"int", TokenType::INT},
    {"unsigned", TokenType::UNSIGNED},
    {"void", TokenType::VOID},
    {"auto", TokenType::AUTO}
};

const std::unordered_set<TokenType> types {
    TokenType::INT,        //< signed integer
    TokenType::UNSIGNED,   //< unsigned integer
    TokenType::AUTO,       //< deducing type automatically
    TokenType::VOID        //< no return type
};