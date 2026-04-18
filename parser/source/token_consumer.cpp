#include "../token_consumer.hpp"

#include <format>
#include <stdexcept>

syntax::TokenConsumer::TokenConsumer(lex::Lexer& lexer) : lexer{ lexer } {} 

void syntax::TokenConsumer::consume(syntax::TokenType expectedType) {
    const auto& token{ getToken() };
    if(token.type != expectedType){
        throw std::runtime_error(
            std::format("Line {}, Column {}: SYNTAX ERROR -> expected '{}', got '{} {}'",
            token.line, 
            token.column, 
            syntax::tokenTypeToStr(expectedType), 
            syntax::tokenTypeToStr(token.type), 
            token.value
        ));
    }
    next();
}

void syntax::TokenConsumer::consume(syntax::GeneralTokenType expectedGType) {
    const auto& token{ getToken() };
    if(token.gtype != expectedGType){
        throw std::runtime_error(
            std::format("Line {}, Column {}: SYNTAX ERROR -> expected '{}', got '{} {}'",
            token.line, 
            token.column, 
            syntax::generalTokenTypeToStr(expectedGType), 
            syntax::generalTokenTypeToStr(token.gtype), 
            token.value
        ));
    }
    next();
}
