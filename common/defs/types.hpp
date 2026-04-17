#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstddef>
#include <array>
#include <string_view>
#include "../token/token_types.hpp"

/** 
 * @enum Type
 * @brief types supported by the compiler
*/
enum class Type {
    NO_TYPE = 0,   //< absence of a type / invalid state
    INT,           //< signed integer
    UNSIGNED,      //< unsigned integer
    VOID,          //< no value / function return type
    AUTO           //< type deduced automatically
};

/// number of types
constexpr size_t TYPE_COUNT{ static_cast<size_t>(Type::AUTO) + 1 };

/// maps types to their string representation
constexpr std::array<std::string_view, TYPE_COUNT> typeStringRepresentations {
    [] {
        /**
         * @brief converts type enum to array index
         * @param type - element of the type enum
         * @returns index in the array for provided type
        */
        constexpr auto idx {
            [](Type type) noexcept -> size_t {
                return static_cast<size_t>(type);
            }
        };
        std::array<std::string_view, TYPE_COUNT> types{};

        types[idx(Type::NO_TYPE)]  = "no_type";
        types[idx(Type::INT)]      = "int";
        types[idx(Type::UNSIGNED)] = "unsigned";
        types[idx(Type::VOID)]     = "void";
        types[idx(Type::AUTO)]     = "auto";

        return types;
    }()
};

/**
 * @brief getter for the string representation of the type
 * @param type - type (e.g. int, unsigned)
 * @returns string representation of the type
*/
constexpr std::string_view typeToStr(Type type){
    return typeStringRepresentations[static_cast<size_t>(type)];
}

/// maps token type to type
constexpr std::array<Type, syntax::TOKEN_TYPE_COUNT> tokenTypeTypeRepresentations {
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

        std::array<Type, syntax::TOKEN_TYPE_COUNT> types{};

        types[idx(syntax::TokenType::INT)]      = Type::INT;
        types[idx(syntax::TokenType::UNSIGNED)] = Type::UNSIGNED;
        types[idx(syntax::TokenType::VOID)]     = Type::VOID;
        types[idx(syntax::TokenType::AUTO)]     = Type::AUTO;

        return types;
    }()
};

/**
 * @brief getter for the type representation of the token type
 * @param type - type of the token
 * @returns type representation of the token type,
 * if token is not type, it returns Type::NO_TYPE
*/
constexpr Type tokenTypeToType(syntax::TokenType type){
    return tokenTypeTypeRepresentations[static_cast<size_t>(type)];
}

#endif