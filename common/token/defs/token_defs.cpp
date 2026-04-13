#include "token_defs.hpp"

const std::unordered_map<TokenType, std::string> tokenTypeToString {
    /// preprocessor / directive tokens
    {TokenType::INCLUDE, "INCLUDE"},
    
    /// control flow tokens
    {TokenType::IF, "IF"},
    {TokenType::ELSE, "ELSE"},
    {TokenType::ASSIGN, "ASSIGN"},
    {TokenType::WHILE, "WHILE"},
    {TokenType::DO, "DO"},
    {TokenType::FOR, "FOR"},
    {TokenType::RETURN, "RETURN"},
    {TokenType::SWITCH, "SWITCH"},
    {TokenType::CASE, "CASE"},
    {TokenType::DEFAULT, "DEFAULT"},
    {TokenType::BREAK, "BREAK"},

    /// identifier and literal tokens
    {TokenType::ID, "ID"},
    {TokenType::LITERAL, "LITERAL"},

    /// arithmetic operators
    {TokenType::PLUS, "PLUS"},
    {TokenType::MINUS, "MINUS"},
    {TokenType::STAR, "STAR"},
    {TokenType::SLASH, "SLASH"},

    /// bitwise operators
    {TokenType::AMPERSEND, "AMPERSEND"},
    {TokenType::PIPE, "PIPE"},
    {TokenType::CARET, "CARET"},
    {TokenType::LSHIFT, "LSHIFT"},
    {TokenType::RSHIFT, "RSHIFT"},

    /// relational operators
    {TokenType::LESS, "LESS"},
    {TokenType::GREATER, "GREATER"},
    {TokenType::LESS_EQ, "LESS_EQ"},
    {TokenType::GREATER_EQ, "GREATER_EQ"},
    {TokenType::EQUAL, "EQUAL"},
    {TokenType::NOT_EQ, "NOT_EQ"},

    /// logical operators
    {TokenType::LOGICAL_AND, "LOGICAL_AND"},
    {TokenType::LOGICAL_OR, "LOGICAL_OR"},

    /// type tokens
    {TokenType::INT, "INT"},
    {TokenType::UNSIGNED, "UNSIGNED"},
    {TokenType::VOID, "VOID"},
    {TokenType::AUTO, "AUTO"},

    /// punctuation tokens
    {TokenType::LPAREN, "LPAREN"},
    {TokenType::RPAREN, "RPAREN"},
    {TokenType::LBRACE, "LBRACE"},
    {TokenType::RBRACE, "RBRACE"},
    {TokenType::SEMICOLON, "SEMICOLON"},
    {TokenType::COLON, "COLON"},
    {TokenType::COMMA, "COMMA"},
    {TokenType::HASH, "HASH"},

    /// special tokens
    {TokenType::INVALID, "INVALID"},
    {TokenType::_EOF, "EOF"}

};

const std::unordered_map<GeneralTokenType, std::string> generalTokenTypeToString {
    {GeneralTokenType::VALUE, "VALUE"},         //< identifiers and literals
    {GeneralTokenType::OPERATOR, "OPERATOR"},   //< all operators
    {GeneralTokenType::TYPE, "TYPE"},           //< type keywords
    {GeneralTokenType::OTHER, "OTHER"}          //< everything else (keywords, punctuations, etc.)
};
