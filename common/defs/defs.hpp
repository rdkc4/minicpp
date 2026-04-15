#ifndef DEFS_HPP
#define DEFS_HPP

#include <array>
#include <string_view>

#include "../token/defs/token_defs.hpp"

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

/** 
 * @enum Kind
 * @brief kinds of symbols
*/
enum class Kind {
    NO_KIND = 0,   //< undefined kind, invalid state
    LIT,           //< literal constant
    FUN,           //< function
    VAR,           //< variable
    PAR            //< function parameter
};

/// number of kinds
constexpr size_t KIND_COUNT{ static_cast<size_t>(Kind::PAR) + 1 };

/** 
 * @enum Operator
 * @brief operators supported by the compiler
*/
enum class Operator {
    NO_OP = 0, //< no operator / invalid state

    /// arithmetic
    ADD,       //< +
    SUB,       //< -
    MUL,       //< *
    DIV,       //< /

    /// bitwise
    ANDB,      //< &
    ORB,       //< |
    XOR,       //< ^
    LSHIFT,    //< <<
    RSHIFT,    //< >>

    /// logical
    ANDL,      //< &&
    ORL,       //< ||

    /// relational
    LESS,      //< <
    GREATER,   //< >
    LEQUAL,    //< <=
    GEQUAL,    //< >=
    EQUAL,     //< ==
    NEQUAL     //< !=
};

/// number of operators
constexpr size_t OPERATOR_COUNT = static_cast<size_t>(Operator::NEQUAL) + 1;

/// maps token type to operator
constexpr std::array<Operator, TOKEN_TYPE_COUNT> tokenTypeOperatorRepresentations {
    [] {
        /**
         * @brief converts token type enum to array index
         * @param type - element of the token type enum
         * @returns index in the array for provided token type
        */
        constexpr auto idx {
            [](TokenType type) -> size_t {
                return static_cast<size_t>(type);
            }
        };

        std::array<Operator, TOKEN_TYPE_COUNT> ops{};

        ops[idx(TokenType::PLUS)]        = Operator::ADD;
        ops[idx(TokenType::MINUS)]       = Operator::SUB;
        ops[idx(TokenType::ASTERISK)]    = Operator::MUL;
        ops[idx(TokenType::SLASH)]       = Operator::DIV;

        ops[idx(TokenType::AMPERSEND)]   = Operator::ANDB;
        ops[idx(TokenType::PIPE)]        = Operator::ORB;
        ops[idx(TokenType::CARET)]       = Operator::XOR;
        ops[idx(TokenType::LSHIFT)]      = Operator::LSHIFT;
        ops[idx(TokenType::RSHIFT)]      = Operator::RSHIFT;

        ops[idx(TokenType::LOGICAL_AND)] = Operator::ANDL;
        ops[idx(TokenType::LOGICAL_OR)]  = Operator::ORL;
        
        ops[idx(TokenType::LESS)]        = Operator::LESS;
        ops[idx(TokenType::GREATER)]     = Operator::GREATER;
        ops[idx(TokenType::LESS_EQ)]     = Operator::LEQUAL;
        ops[idx(TokenType::GREATER_EQ)]  = Operator::GEQUAL;
        ops[idx(TokenType::EQUAL)]       = Operator::EQUAL;
        ops[idx(TokenType::NOT_EQ)]      = Operator::NEQUAL;

        return ops;
    }()
};

/**
 * @brief getter for the operator of the underlying token type
 * @param type - type of the token
 * @returns operator of the underlying token type,
 * if token type is not an operator, returns Operator::NO_OP
*/
constexpr Operator tokenTypeToOperator(TokenType type){
    return tokenTypeOperatorRepresentations[static_cast<size_t>(type)];
}

/// maps types to their string representation
constexpr std::array<std::string_view, TYPE_COUNT> typeStringRepresentations {
    [] {
        /**
         * @brief converts type enum to array index
         * @param type - element of the type enum
         * @returns index in the array for provided type
        */
        constexpr auto idx {
            [](Type type) -> size_t {
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
constexpr std::array<Type, TOKEN_TYPE_COUNT> tokenTypeTypeRepresentations {
    [] {
        /**
         * @brief converts token type enum to array index
         * @param type - element of the token type enum
         * @returns index in the array for provided token type
        */
        constexpr auto idx {
            [](TokenType type) -> size_t {
                return static_cast<size_t>(type);
            }
        };

        std::array<Type, TOKEN_TYPE_COUNT> types{};

        types[idx(TokenType::INT)]      = Type::INT;
        types[idx(TokenType::UNSIGNED)] = Type::UNSIGNED;
        types[idx(TokenType::VOID)]     = Type::VOID;
        types[idx(TokenType::AUTO)]     = Type::AUTO;

        return types;
    }()
};

/**
 * @brief getter for the type representation of the token type
 * @param type - type of the token
 * @returns type representation of the token type,
 * if token is not type, it returns Type::NO_TYPE
*/
constexpr Type tokenTypeToType(TokenType type){
    return tokenTypeTypeRepresentations[static_cast<size_t>(type)];
}

/// maps kinds to their string representations
constexpr std::array<std::string_view, KIND_COUNT> kindStringRepresentations {
    [] {
        /**
         * @brief converts kind enum to array index
         * @param kind - element of the kind enum
         * @returns index in the array for provided kind
        */
        constexpr auto idx {
            [](Kind kind) -> size_t {
                return static_cast<size_t>(kind);
            }
        };

        std::array<std::string_view, KIND_COUNT> kinds{};

        kinds[idx(Kind::NO_KIND)] = "NO_KIND";
        kinds[idx(Kind::LIT)]     = "LIT";
        kinds[idx(Kind::FUN)]     = "FUN";
        kinds[idx(Kind::VAR)]     = "VAR";
        kinds[idx(Kind::PAR)]     = "PAR";

        return kinds;
    }()
};

/**
 * @brief getter for the string representation of the kind
 * @param kind - kind of the token
 * @returns string representation of the kind
*/
constexpr std::string_view kindToStr(Kind kind){
    return kindStringRepresentations[static_cast<size_t>(kind)];
}

/// maps operators to their string representations
constexpr std::array<std::string_view, OPERATOR_COUNT> operatorStringRepresentations {
    [] {
        /**
         * @brief converts operator enum to array index
         * @param op - element of the operator enum
         * @returns index in the array for provided operator
        */
        constexpr auto idx {
            [](Operator op) -> size_t {
                return static_cast<size_t>(op);
            }
        };

        std::array<std::string_view, OPERATOR_COUNT> ops;

        ops[idx(Operator::NO_OP)]   = "NO_OP";

        ops[idx(Operator::ADD)]     = "+";
        ops[idx(Operator::SUB)]     = "-";
        ops[idx(Operator::MUL)]     = "*";
        ops[idx(Operator::DIV)]     = "/";
        
        ops[idx(Operator::ANDB)]    = "&";
        ops[idx(Operator::ORB)]     = "|";
        ops[idx(Operator::XOR)]     = "^";
        ops[idx(Operator::LSHIFT)]  = "<<";
        ops[idx(Operator::RSHIFT)]  = ">>";

        ops[idx(Operator::ANDL)]    = "&&";
        ops[idx(Operator::ORL)]     = "||";

        ops[idx(Operator::LESS)]    = "<";
        ops[idx(Operator::GREATER)] = ">";
        ops[idx(Operator::LEQUAL)]  = "<=";
        ops[idx(Operator::GEQUAL)]  = ">=";
        ops[idx(Operator::EQUAL)]   = "==";
        ops[idx(Operator::NEQUAL)]  = "!=";

        return ops;
    }()
};

/**
 * @brief getter for the string representation of the operator
 * @param op - operator
 * @returns string representation of the operator
*/
constexpr std::string_view operatorToStr(Operator op){
    return operatorStringRepresentations[static_cast<size_t>(op)];
}

#endif
