#include "token.hpp"

Token::Token() : value{ "" }, line{ 0 }, column{ 0 }, type{ TokenType::_NODE } {}

Token::Token(std::string_view value, size_t line, size_t column) : value{ value }, line{ line }, column{ column }, type{ TokenType::_NODE } {}

Token::Token(std::string_view value, size_t line, size_t column, TokenType type) : value{ value }, line{ line }, column{ column }, type{ type } {}

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
    {TokenType::_INVALID, "INVALID"}
};