#include "code_generator_defs.hpp"

// jumps
const std::unordered_map<std::string, std::vector<std::string>> stringToJMP {
    {">", {"jg", "ja"}},
    {">=", {"jge", "jae"}},
    {"<", {"jl", "jb"}},
    {"<=", {"jle", "jbe"}},
    {"==", {"je", "je"}},
    {"!=", {"jne", "jne"}}
};

// opposite jumps
const std::unordered_map<std::string, std::vector<std::string>> stringToOppJMP {
    {">", {"jle", "jbe"}},
    {">=", {"jl", "jb"}},
    {"<", {"jge", "jae"}},
    {"<=", {"jg", "ja"}},
    {"==", {"jne", "jne"}},
    {"!=", {"je", "je"}}
};

// general purpose registers (x86_64 architecture)
const std::vector<std::string> gpRegisters {"%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"};