#include "token.hpp"

syntax::Token::Token() 
    : value{ "" }, 
      line{ 0 }, 
      column{ 0 }, 
      type{ syntax::TokenType::NODE }, 
      gtype{ syntax::GeneralTokenType::OTHER } {}

syntax::Token::Token(std::string_view value, size_t line, size_t column) 
    : value{ value }, 
      line{ line }, 
      column{ column }, 
      type{ syntax::TokenType::NODE }, 
      gtype{ syntax::GeneralTokenType::OTHER } {}

syntax::Token::Token(std::string_view value, size_t line, size_t column, syntax::TokenType type) 
    : value{ value }, 
      line{ line }, 
      column{ column }, 
      type{ type }, 
      gtype{ syntax::GeneralTokenType::OTHER } {}

syntax::Token::Token(std::string_view value, size_t line, size_t column, syntax::TokenType type, syntax::GeneralTokenType gtype) 
    : value{ value }, 
      line{ line }, 
      column{ column }, 
      type{ type }, 
      gtype{ gtype } {}
