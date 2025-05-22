#include "token.hpp"

Token::Token() : type{TokenType::_NODE}, value{""}, line{0}, column{0} {}

Token::Token(std::string value, size_t line, size_t column) : type{TokenType::_NODE}, value{value}, line{line}, column{column} {}

Token::Token(TokenType type, const std::string& value, size_t line, size_t column): type{type}, value{value}, line{line}, column{column} {}

const std::unordered_map<TokenType, std::string> tokenTypeToString {
    {TokenType::_ID, "ID"},
    {TokenType::_LITERAL, "LITERAL"},
    {TokenType::_AROP, "AROP"},
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