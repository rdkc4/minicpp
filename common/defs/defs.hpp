#ifndef DEFS_HPP
#define DEFS_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

enum class Types{INT, UNSIGNED, VOID, NO_TYPE};

enum class Kinds{NO_KIND, REG, LIT, FUN, VAR, PAR};

extern const std::unordered_set<std::string> relationalOperators;

extern const std::unordered_set<std::string> arithmeticOperators;

extern const std::unordered_set<std::string> bitwiseOperators;

extern const std::unordered_map<Types, std::string> typeToString;

extern const std::unordered_map<std::string, Types> stringToType;

extern const std::unordered_map<Kinds, std::string> kindToString;

extern const std::unordered_map<std::string, std::vector<std::string>> stringToJMP;

extern const std::unordered_map<std::string, std::vector<std::string>> stringToOppJMP;

#endif
