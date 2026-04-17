#ifndef TOKEN_CONSUMER_HPP
#define TOKEN_CONSUMER_HPP

#include "../lexer/lexer.hpp"

/**
 * @class TokenConsumer
 * @brief Lexer wrapper, shared across parsers, non-owning
*/
class TokenConsumer {
public:
    /** 
     * @brief Creates instance of the token consumer
     * @param lexer - reference to the lexer
     * @note token consumer is a wrapper around the lexer
    */
    TokenConsumer(Lexer& lexer);
    
    /** 
     * @brief moving on to the next token
    */
    inline void next() noexcept {
        lexer.next();
    }

    /** 
     * @brief peeking at the next token
     * @returns const reference to the next token
    */
    inline const syntax::Token& peek() const noexcept {
        return lexer.peek();
    }

    /** 
     * @brief compares current token's type to expected
     * @throws if token types don't match
     * @param expectedType - token type that is expected to appear next
    */
    void consume(syntax::TokenType expectedType);

    /** 
     * @brief compares current token's general type to expected
     * @throws if token general types don't match
     * @param expectedGType - general token type that is expected to appear next
    */
    void consume(syntax::GeneralTokenType expectedGType);

    /** 
     * @brief getter for the token currently processed
     * @return const reference to a currently processed token
    */
    inline const syntax::Token& getToken() const noexcept {
        return lexer.current();
    }

private:
    /// reference to a lexer containing tokenized input
    Lexer& lexer;
};

#endif