#include "../token_consumer.hpp"

#include <format>
#include <stdexcept>

TokenConsumer::TokenConsumer(Lexer& lexer) : lexer{ lexer } {} 

void TokenConsumer::consume(syntax::TokenType expectedType) {
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

void TokenConsumer::consume(syntax::GeneralTokenType expectedGType) {
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
