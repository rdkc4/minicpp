#ifndef DEFS_HPP
#define DEFS_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../token/defs/token_defs.hpp"

/** 
 * @enum Types
 * @brief types supported by the compiler
*/
enum class Types{NO_TYPE, INT, UNSIGNED, VOID, AUTO};

/** 
 * @enum Kinds
 * @brief kinds of symbols
*/
enum class Kinds{NO_KIND, LIT, FUN, VAR, PAR};

/** 
 * @enum Operators
 * @brief operators supported by the compiler
*/
enum class Operators{NO_OP, ADD, SUB, MUL, DIV, ANDB, ORB, XOR, LSHIFT, RSHIFT, GREATER, LESS, LEQUAL, GEQUAL, EQUAL, NEQUAL};

/// set of relational operators as strings
extern const std::unordered_set<std::string> relationalOperators;

/// set of arithmetic operators as strings
extern const std::unordered_set<std::string> arithmeticOperators;

/// set of bitwise operators as strings
extern const std::unordered_set<std::string> bitwiseOperators;

/// maps string to an operator
extern const std::unordered_map<std::string, Operators> stringToOperator;

/// maps type to a string
extern const std::unordered_map<Types, std::string> typeToString;

/// maps token type to a type
extern const std::unordered_map<TokenType, Types> tokenTypeToType;

/// maps kind to a string
extern const std::unordered_map<Kinds, std::string> kindToString;

/// maps operator to a string
extern const std::unordered_map<Operators, std::string> operatorToString;

#endif
