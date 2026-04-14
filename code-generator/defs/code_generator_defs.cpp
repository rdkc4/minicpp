#include "code_generator_defs.hpp"

const std::unordered_map<IR::defs::IRNodeType, std::string> nodeToJMP {
    {IR::defs::IRNodeType::JG, "jg"},
    {IR::defs::IRNodeType::JA, "ja"},
    {IR::defs::IRNodeType::JL, "jl"},
    {IR::defs::IRNodeType::JB, "jb"},
    {IR::defs::IRNodeType::JGE, "jge"},
    {IR::defs::IRNodeType::JAE, "jae"},
    {IR::defs::IRNodeType::JLE, "jle"},
    {IR::defs::IRNodeType::JBE, "jbe"},
    {IR::defs::IRNodeType::JE, "je"},
    {IR::defs::IRNodeType::JNE, "jne"}
};

const std::unordered_map<IR::defs::IRNodeType, std::string> nodeToOppJMP {
    {IR::defs::IRNodeType::JG, "jle"},
    {IR::defs::IRNodeType::JA, "jbe"},
    {IR::defs::IRNodeType::JL, "jge"},
    {IR::defs::IRNodeType::JB, "jae"},
    {IR::defs::IRNodeType::JGE, "jl"},
    {IR::defs::IRNodeType::JAE, "jb"},
    {IR::defs::IRNodeType::JLE, "jg"},
    {IR::defs::IRNodeType::JBE, "ja"},
    {IR::defs::IRNodeType::JE, "jne"},
    {IR::defs::IRNodeType::JNE, "je"}
};

const std::unordered_map<IR::defs::IRNodeType, std::string> nodeToSetExt {
    {IR::defs::IRNodeType::JG, "g"},
    {IR::defs::IRNodeType::JA, "a"},
    {IR::defs::IRNodeType::JL, "l"},
    {IR::defs::IRNodeType::JB, "b"},
    {IR::defs::IRNodeType::JGE, "ge"},
    {IR::defs::IRNodeType::JAE, "ae"},
    {IR::defs::IRNodeType::JLE, "le"},
    {IR::defs::IRNodeType::JBE, "be"},
    {IR::defs::IRNodeType::JE, "e"},
    {IR::defs::IRNodeType::JNE, "ne"}
};

/// general purpose registers (x86_64 architecture)
const std::vector<std::string> gpRegisters {
    "%r8", 
    "%r9", 
    "%r10", 
    "%r11", 
    "%r12", 
    "%r13", 
    "%r14", 
    "%r15"
};