#ifndef CODE_GENERATOR_DEFS_HPP
#define CODE_GENERATOR_DEFS_HPP

#include <unordered_map>
#include <vector>
#include <string>

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// definitions related to code generation
//-----------------------------------------------------------------------------------------------------------------------------------------------------

extern const std::unordered_map<std::string, std::vector<std::string>> stringToJMP;

extern const std::unordered_map<std::string, std::vector<std::string>> stringToOppJMP;

extern const std::vector<std::string> gpRegisters;

#endif
