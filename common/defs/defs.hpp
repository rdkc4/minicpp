#ifndef DEFS_HPP
#define DEFS_HPP

#include <string>
#include <unordered_map>
#include <unordered_set>

/*
    global definitions
*/

enum class Types{INT, UNSIGNED, VOID, AUTO, NO_TYPE};

enum class Kinds{NO_KIND, REG, LIT, FUN, VAR, PAR};

extern const std::unordered_set<std::string> relationalOperators;

extern const std::unordered_set<std::string> arithmeticOperators;

extern const std::unordered_set<std::string> bitwiseOperators;

extern const std::unordered_map<Types, std::string> typeToString;

extern const std::unordered_map<std::string, Types> stringToType;

extern const std::unordered_map<Kinds, std::string> kindToString;

#endif
