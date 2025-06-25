#include "token_defs.hpp"

const std::unordered_map<TokenType, std::string> tokenTypeToString {
    {TokenType::_ID, "ID"},
    {TokenType::_LITERAL, "LITERAL"},
    {TokenType::_AROP, "AROP"},
    {TokenType::_BITWISE, "BITWISE"},
    {TokenType::_RELOP, "RELOP"},
    {TokenType::_LPAREN, "LPAREN"},
    {TokenType::_RPAREN, "RPAREN"},
    {TokenType::_LBRACKET, "LBRACKET"},
    {TokenType::_RBRACKET, "RBRACKET"},
    {TokenType::_SEMICOLON, "SEMICOLON"},
    {TokenType::_COLON, "COLON"},
    {TokenType::_ASSIGN, "ASSIGN"},
    {TokenType::_TYPE, "TYPE"},
    {TokenType::_IF, "IF"},
    {TokenType::_ELSE, "ELSE"},
    {TokenType::_WHILE, "WHILE"},
    {TokenType::_FOR, "FOR"},
    {TokenType::_DO, "DO"},
    {TokenType::_SWITCH, "SWITCH"},
    {TokenType::_CASE, "CASE"},
    {TokenType::_DEFAULT, "DEFAULT"},
    {TokenType::_BREAK, "BREAK"},
    {TokenType::_RETURN, "RETURN"},
    {TokenType::_COMMA, "COMMA"},
    {TokenType::_EOF, "EOF"},
    {TokenType::_INVALID, "INVALID"},
    {TokenType::_PRINTF, "PRINTF"}
};

const std::unordered_map<GeneralTokenType, std::string> generalTokenTypeToString {
    {GeneralTokenType::VALUE, "VALUE"},
    {GeneralTokenType::OPERATOR, "OPERATOR"},
    {GeneralTokenType::OTHER, "OTHER"}
};
