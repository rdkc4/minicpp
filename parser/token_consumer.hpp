#ifndef TOKEN_CONSUMER_HPP
#define TOKEN_CONSUMER_HPP

#include "../lexer/lexer.hpp"

class TokenConsumer {
public:
    TokenConsumer(Lexer& lexer);

protected:
    const Token& next() noexcept;
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