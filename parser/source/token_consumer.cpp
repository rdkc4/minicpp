#include "../token_consumer.hpp"

#include <format>
#include <stdexcept>

TokenConsumer::TokenConsumer(Lexer& lexer) : lexer{ lexer } {} 

void TokenConsumer::next() noexcept {
    lexer.next();
}
const Token& TokenConsumer::peek() const noexcept {
    return lexer.peek();
}

// expecting specific type of token
void TokenConsumer::consume(TokenType expectedType) {
    if(getToken().type != expectedType){
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> expected '{}', got '{} {}'",
            getToken().line, getToken().column, tokenTypeToString.at(expectedType), tokenTypeToString.at(getToken().type), getToken().value));
    }
    next();
}

// expecting general type of token
void TokenConsumer::consume(GeneralTokenType expectedGType) {
    if(getToken().gtype != expectedGType){
        throw std::runtime_error(std::format("Line {}, Column {}: SYNTAX ERROR -> expected '{}', got '{} {}'",
            getToken().line, getToken().column, generalTokenTypeToString.at(expectedGType), generalTokenTypeToString.at(getToken().gtype), getToken().value));
    }
    next();
}
