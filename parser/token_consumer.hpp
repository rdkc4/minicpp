#ifndef TOKEN_CONSUMER_HPP
#define TOKEN_CONSUMER_HPP

#include "../lexer/lexer.hpp"

/*
    Lexer wrapper
    Shared across parsers
    Non-owning
*/
class TokenConsumer {
public:
    TokenConsumer(Lexer& lexer);
    
    void next() noexcept;
    const Token& peek() const noexcept;

    void consume(TokenType expectedType);
    void consume(GeneralTokenType expectedGType);

    inline const Token& getToken() const noexcept {
        return lexer.current();
    }

private:
    Lexer& lexer;
};

#endif