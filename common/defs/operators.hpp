#ifndef OPERATORS_HPP
#define OPERATORS_HPP

#include <cstddef>
#include "../token/token_types.hpp"

namespace syntax {
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
    constexpr size_t OPERATOR_COUNT{ static_cast<size_t>(Operator::NEQUAL) + 1 };

    /// maps operators to their string representations
    constexpr std::array<std::string_view, OPERATOR_COUNT> operatorStringRepresentations {
        [] {
            /**
             * @brief converts operator enum to array index
             * @param op - element of the operator enum
             * @returns index in the array for provided operator
            */
            constexpr auto idx {
                [](Operator op) noexcept -> size_t {
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

    /// maps token type to operator
    constexpr std::array<Operator, syntax::TOKEN_TYPE_COUNT> tokenTypeOperatorRepresentations {
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

            std::array<Operator, syntax::TOKEN_TYPE_COUNT> ops{};

            ops[idx(syntax::TokenType::PLUS)]        = Operator::ADD;
            ops[idx(syntax::TokenType::MINUS)]       = Operator::SUB;
            ops[idx(syntax::TokenType::ASTERISK)]    = Operator::MUL;
            ops[idx(syntax::TokenType::SLASH)]       = Operator::DIV;

            ops[idx(syntax::TokenType::AMPERSEND)]   = Operator::ANDB;
            ops[idx(syntax::TokenType::PIPE)]        = Operator::ORB;
            ops[idx(syntax::TokenType::CARET)]       = Operator::XOR;
            ops[idx(syntax::TokenType::LSHIFT)]      = Operator::LSHIFT;
            ops[idx(syntax::TokenType::RSHIFT)]      = Operator::RSHIFT;

            ops[idx(syntax::TokenType::LOGICAL_AND)] = Operator::ANDL;
            ops[idx(syntax::TokenType::LOGICAL_OR)]  = Operator::ORL;
            
            ops[idx(syntax::TokenType::LESS)]        = Operator::LESS;
            ops[idx(syntax::TokenType::GREATER)]     = Operator::GREATER;
            ops[idx(syntax::TokenType::LESS_EQ)]     = Operator::LEQUAL;
            ops[idx(syntax::TokenType::GREATER_EQ)]  = Operator::GEQUAL;
            ops[idx(syntax::TokenType::EQUAL)]       = Operator::EQUAL;
            ops[idx(syntax::TokenType::NOT_EQ)]      = Operator::NEQUAL;

            return ops;
        }()
    };

    /**
     * @brief getter for the operator of the underlying token type
     * @param type - type of the token
     * @returns operator of the underlying token type,
     * if token type is not an operator, returns Operator::NO_OP
    */
    constexpr Operator tokenTypeToOperator(syntax::TokenType type){
        return tokenTypeOperatorRepresentations[static_cast<size_t>(type)];
    }

}

#endif