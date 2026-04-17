#ifndef PARSER_DEFS_HPP
#define PARSER_DEFS_HPP

#include <array>
#include "../../common/token/token_types.hpp"

/// precedence of the token types
constexpr std::array<int, syntax::TOKEN_TYPE_COUNT> precedence { 
    [] {
        /**
         * @brief converts token type enum to array index
         * @param type - element of the token type enum
         * @returns index in the array for provided token type
        */
        constexpr auto idx{
            [](syntax::TokenType type) -> size_t {
                return static_cast<size_t>(type);
            }
        };

        std::array<int, syntax::TOKEN_TYPE_COUNT> prec{};

        prec[idx(syntax::TokenType::LOGICAL_OR)]  = 1;
        prec[idx(syntax::TokenType::LOGICAL_AND)] = 2;
        prec[idx(syntax::TokenType::PIPE)]        = 3;
        prec[idx(syntax::TokenType::CARET)]       = 4;
        prec[idx(syntax::TokenType::AMPERSEND)]   = 5;
        prec[idx(syntax::TokenType::EQUAL)]       = 6;
        prec[idx(syntax::TokenType::NOT_EQ)]      = 6;
        prec[idx(syntax::TokenType::LESS)]        = 7;
        prec[idx(syntax::TokenType::LESS_EQ)]     = 7;
        prec[idx(syntax::TokenType::GREATER)]     = 7;
        prec[idx(syntax::TokenType::GREATER_EQ)]  = 7;
        prec[idx(syntax::TokenType::LSHIFT)]      = 8;
        prec[idx(syntax::TokenType::RSHIFT)]      = 8;
        prec[idx(syntax::TokenType::PLUS)]        = 9;
        prec[idx(syntax::TokenType::MINUS)]       = 9;
        prec[idx(syntax::TokenType::ASTERISK)]    = 10;
        prec[idx(syntax::TokenType::SLASH)]       = 10;

        return prec;
    }()
};

/**
 * @brief getter for the precedence of the token
 * @param type - type of the token
 * @returns number representing the precedence of the token
*/
constexpr int getPrecedence(syntax::TokenType type) {
    return precedence[static_cast<size_t>(type)];
}

static_assert(precedence.size() == syntax::TOKEN_TYPE_COUNT);

#endif