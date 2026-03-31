#ifndef DEFS_HPP
#define DEFS_HPP

#include <string>
#include <unordered_map>

#include "../token/defs/token_defs.hpp"

/** 
 * @enum Type
 * @brief types supported by the compiler
*/
enum class Type{NO_TYPE, INT, UNSIGNED, VOID, AUTO};

/** 
 * @enum Kind
 * @brief kinds of symbols
*/
enum class Kind{NO_KIND, LIT, FUN, VAR, PAR};

/** 
 * @enum Operator
 * @brief operators supported by the compiler
*/
enum class Operator{NO_OP, ADD, SUB, MUL, DIV, ANDB, ORB, XOR, LSHIFT, RSHIFT, GREATER, LESS, LEQUAL, GEQUAL, EQUAL, NEQUAL};

/// maps string to an operator
extern const std::unordered_map<std::string, Operator> stringToOperator;

/// maps type to a string
extern const std::unordered_map<Type, std::string> typeToString;

/// maps token type to a type
extern const std::unordered_map<TokenType, Type> tokenTypeToType;

/// maps kind to a string
extern const std::unordered_map<Kind, std::string> kindToString;

/// maps operator to a string
extern const std::unordered_map<Operator, std::string> operatorToString;

#endif
