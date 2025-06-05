#ifndef CODE_GENERATOR_DEFS_HPP
#define CODE_GENERATOR_DEFS_HPP

#include <unordered_map>
#include <vector>
#include <string>

/*
    code generation related definitions
*/

extern const std::unordered_map<std::string, std::vector<std::string>> stringToJMP;

extern const std::unordered_map<std::string, std::vector<std::string>> stringToOppJMP;

extern const std::vector<std::string> gpRegisters;

#endif
