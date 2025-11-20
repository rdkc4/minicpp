#ifndef TOKEN_CONSUMER_HPP
#define TOKEN_CONSUMER_HPP

#include "../lexer/lexer.hpp"

/**
 * @class TokenConsumer
 * @brief Lexer wrapper
 * 
 * shared across parsers
 *
 * non-owning
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
     * @returns void
    */
    void next() noexcept;

    /** 
     * @brief peeking at the next token
     * @returns const reference to the next token
    */
    const Token& peek() const noexcept;

    /** 
     * @brief compares current token's type to expected
     * @throws if token types don't match
     * @param expectedType - token type that is expected to appear next
     * @returns void
    */
    void consume(TokenType expectedType);

    /** 
     * @brief compares current token's general type to expected
     * @throws if token general types don't match
     * @param expectedGType - general token type that is expected to appear next
     * @returns void
    */
    void consume(GeneralTokenType expectedGType);

    /** 
     * @brief getter for the token currently processed
     * @return const reference to a currently processed token
    */
    inline const Token& getToken() const noexcept {
        return lexer.current();
    }

private:
    /// reference to a lexer containing tokenized input
    Lexer& lexer;
};

#endif