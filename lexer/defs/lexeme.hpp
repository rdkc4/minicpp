#ifndef LEXEME_HPP
#define LEXEME_HPP

#include <cstddef>

#include "../../common/token/defs/token_defs.hpp"

/**
 * @struct Lexeme
 * @brief describes the basic lexical unit
*/
struct Lexeme {
    /// type of the token.
    TokenType type;

    /// length of the token, invalid if 0.
    size_t length;
};

#endif