#include "token.hpp"

Token::Token() : value{ "" }, line{ 0 }, column{ 0 }, type{ TokenType::_NODE }, gtype{ GeneralTokenType::OTHER } {}

Token::Token(std::string_view value, size_t line, size_t column) : 
    value{ value }, line{ line }, column{ column }, type{ TokenType::_NODE }, gtype{ GeneralTokenType::OTHER } {}

Token::Token(std::string_view value, size_t line, size_t column, TokenType type) : 
    value{ value }, line{ line }, column{ column }, type{ type }, gtype{ GeneralTokenType::OTHER } {}

Token::Token(std::string_view value, size_t line, size_t column, TokenType type, GeneralTokenType gtype) : 
    value{ value }, line{ line }, column{ column }, type{ type }, gtype{ gtype } {}
