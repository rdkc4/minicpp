#include "../token_consumer.hpp"

#include <format>
#include <stdexcept>

TokenConsumer::TokenConsumer(Lexer& lexer) : lexer{ lexer } {} 

void TokenConsumer::consume(TokenType expectedType) {
    const auto& token{ getToken() };
    if(token.type != expectedType){
        throw std::runtime_error(
            std::format("Line {}, Column {}: SYNTAX ERROR -> expected '{}', got '{} {}'",
            token.line, 
            token.column, 
            tokenTypeToString.at(expectedType), 
            tokenTypeToString.at(token.type), 
            token.value
        ));
    }
    next();
}

void TokenConsumer::consume(GeneralTokenType expectedGType) {
    const auto& token{ getToken() };
    if(token.gtype != expectedGType){
        throw std::runtime_error(
            std::format("Line {}, Column {}: SYNTAX ERROR -> expected '{}', got '{} {}'",
            token.line, 
            token.column, 
            generalTokenTypeToString.at(expectedGType), 
            generalTokenTypeToString.at(token.gtype), 
            token.value
        ));
    }
    next();
}
