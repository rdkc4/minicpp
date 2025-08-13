#ifndef DEFS_HPP
#define DEFS_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../token/defs/token_defs.hpp"

/*
    global definitions
*/

enum class Types{NO_TYPE, INT, UNSIGNED, VOID, AUTO};

enum class Kinds{NO_KIND, REG, LIT, FUN, VAR, PAR};

enum class Operators{NO_OP, ADD, SUB, MUL, DIV, ANDB, ORB, XOR, LSHIFT, RSHIFT, GREATER, LESS, LEQUAL, GEQUAL, EQUAL, NEQUAL};

extern const std::unordered_set<std::string> relationalOperators;

extern const std::unordered_set<std::string> arithmeticOperators;

extern const std::unordered_set<std::string> bitwiseOperators;

extern const std::unordered_map<std::string, Operators> stringToOperator;

extern const std::unordered_map<Types, std::string> typeToString;

extern const std::unordered_map<TokenType, Types> tokenTypeToType;

extern const std::unordered_map<Kinds, std::string> kindToString;

extern const std::unordered_map<Operators, std::string> operatorToString;

#endif
