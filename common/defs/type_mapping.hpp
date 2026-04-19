#ifndef TYPE_MAPPING_HPP
#define TYPE_MAPPING_HPP

#include <array>
#include <cstddef>

#include "types.hpp"
#include "../token/token_types.hpp"

namespace syntax {
    /// maps token type to type
    constexpr std::array<types::Type, TOKEN_TYPE_COUNT> tokenTypeTypeRepresentations {
        [] {
            /**
             * @brief converts token type enum to array index
             * @param type - element of the token type enum
             * @returns index in the array for provided token type
            */
            constexpr auto idx {
                [](syntax::TokenType type) noexcept -> size_t {
                    return static_cast<size_t>(type);
                }
            };

            std::array<types::Type, syntax::TOKEN_TYPE_COUNT> types{};
            types.fill(types::Type::NO_TYPE);

            types[idx(TokenType::INT)]      = types::Type::INT;
            types[idx(TokenType::UNSIGNED)] = types::Type::UNSIGNED;
            types[idx(TokenType::VOID)]     = types::Type::VOID;
            types[idx(TokenType::AUTO)]     = types::Type::AUTO;

            return types;
        }()
    };

    /**
    * @brief getter for the type representation of the token type
    * @param type - type of the token
    * @returns type representation of the token type,
    * if token is not type, it returns Type::NO_TYPE
    */
    constexpr types::Type tokenTypeToType(TokenType type){
        return tokenTypeTypeRepresentations[static_cast<size_t>(type)];
    }

}

#endif