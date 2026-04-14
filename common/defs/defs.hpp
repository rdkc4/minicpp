#ifndef DEFS_HPP
#define DEFS_HPP

#include <string>
#include <unordered_map>

#include "../token/defs/token_defs.hpp"

/** 
 * @enum Type
 * @brief types supported by the compiler
*/
enum class Type {
    NO_TYPE,   //< absence of a type / invalid state
    INT,       //< signed integer
    UNSIGNED,  //< unsigned integer
    VOID,      //< no value / function return type
    AUTO       //< type deduced automatically
};

/** 
 * @enum Kind
 * @brief kinds of symbols
*/
enum class Kind {
    NO_KIND,   //< undefined kind, invalid state
    LIT,       //< literal constant
    FUN,       //< function
    VAR,       //< variable
    PAR        //< function parameter
};

/** 
 * @enum Operator
 * @brief operators supported by the compiler
*/
enum class Operator {
    NO_OP,     //< no operator / invalid state

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

/// maps string to an operator
extern const std::unordered_map<TokenType, Operator> tokenTypeToOperator;

/// maps type to a string
extern const std::unordered_map<Type, std::string> typeToString;

/// maps token type to a type
extern const std::unordered_map<TokenType, Type> tokenTypeToType;

/// maps kind to a string
extern const std::unordered_map<Kind, std::string> kindToString;

/// maps operator to a string
extern const std::unordered_map<Operator, std::string> operatorToString;

#endif
