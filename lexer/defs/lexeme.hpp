#ifndef LEXEME_HPP
#define LEXEME_HPP

#include <cstddef>

#include "../../common/token/token_types.hpp"

/**
 * @struct Lexeme
 * @brief describes the basic lexical unit
*/
struct Lexeme {
    /// type of the token.
    syntax::TokenType type;

    /// length of the token, invalid if 0.
    size_t length;
};

#endif