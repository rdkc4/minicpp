#include "lexer_defs.hpp"

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// keywords reserved of the programming language
//-----------------------------------------------------------------------------------------------------------------------------------------------------
const std::unordered_map<std::string, TokenType> keywords {
    {"return", TokenType::_RETURN},
    {"if", TokenType::_IF},
    {"else", TokenType::_ELSE},
    {"int", TokenType::_TYPE},
    {"unsigned", TokenType::_TYPE},
    {"void", TokenType::_TYPE},
    {"auto", TokenType::_TYPE},
    {"while", TokenType::_WHILE},
    {"for", TokenType::_FOR},
    {"do", TokenType::_DO},
    {"switch", TokenType::_SWITCH},
    {"case", TokenType::_CASE},
    {"default", TokenType::_DEFAULT},
    {"break", TokenType::_BREAK}
};