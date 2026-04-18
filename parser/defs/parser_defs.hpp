#ifndef PARSER_DEFS_HPP
#define PARSER_DEFS_HPP

#include <array>
#include "../../common/token/token_types.hpp"

namespace syntax {
    /// precedence of the token types
    constexpr std::array<int, TOKEN_TYPE_COUNT> precedence { 
        [] {
            /**
             * @brief converts token type enum to array index
             * @param type - element of the token type enum
             * @returns index in the array for provided token type
            */
            constexpr auto idx{
                [](TokenType type) -> size_t {
                    return static_cast<size_t>(type);
                }
            };

            std::array<int, TOKEN_TYPE_COUNT> prec{};

            prec[idx(TokenType::LOGICAL_OR)]  = 1;
            prec[idx(TokenType::LOGICAL_AND)] = 2;
            prec[idx(TokenType::PIPE)]        = 3;
            prec[idx(TokenType::CARET)]       = 4;
            prec[idx(TokenType::AMPERSEND)]   = 5;
            prec[idx(TokenType::EQUAL)]       = 6;
            prec[idx(TokenType::NOT_EQ)]      = 6;
            prec[idx(TokenType::LESS)]        = 7;
            prec[idx(TokenType::LESS_EQ)]     = 7;
            prec[idx(TokenType::GREATER)]     = 7;
            prec[idx(TokenType::GREATER_EQ)]  = 7;
            prec[idx(TokenType::LSHIFT)]      = 8;
            prec[idx(TokenType::RSHIFT)]      = 8;
            prec[idx(TokenType::PLUS)]        = 9;
            prec[idx(TokenType::MINUS)]       = 9;
            prec[idx(TokenType::ASTERISK)]    = 10;
            prec[idx(TokenType::SLASH)]       = 10;

            return prec;
        }()
    };

    /**
     * @brief getter for the precedence of the token
     * @param type - type of the token
     * @returns number representing the precedence of the token
    */
    constexpr int getPrecedence(TokenType type) {
        return precedence[static_cast<size_t>(type)];
    }

    static_assert(precedence.size() == TOKEN_TYPE_COUNT);

}

#endif