#include "code_generator_defs.hpp"

const std::unordered_map<IRNodeType, std::string> nodeToJMP {
    {IRNodeType::JG, "jg"},
    {IRNodeType::JA, "ja"},
    {IRNodeType::JL, "jl"},
    {IRNodeType::JB, "jb"},
    {IRNodeType::JGE, "jge"},
    {IRNodeType::JAE, "jae"},
    {IRNodeType::JLE, "jle"},
    {IRNodeType::JBE, "jbe"},
    {IRNodeType::JE, "je"},
    {IRNodeType::JNE, "jne"}
};

const std::unordered_map<IRNodeType, std::string> nodeToOppJMP {
    {IRNodeType::JG, "jle"},
    {IRNodeType::JA, "jbe"},
    {IRNodeType::JL, "jge"},
    {IRNodeType::JB, "jae"},
    {IRNodeType::JGE, "jl"},
    {IRNodeType::JAE, "jb"},
    {IRNodeType::JLE, "jg"},
    {IRNodeType::JBE, "ja"},
    {IRNodeType::JE, "jne"},
    {IRNodeType::JNE, "je"}
};

const std::unordered_map<IRNodeType, std::string> nodeToSetExt {
    {IRNodeType::JG, "g"},
    {IRNodeType::JA, "a"},
    {IRNodeType::JL, "l"},
    {IRNodeType::JB, "b"},
    {IRNodeType::JGE, "ge"},
    {IRNodeType::JAE, "ae"},
    {IRNodeType::JLE, "le"},
    {IRNodeType::JBE, "be"},
    {IRNodeType::JE, "e"},
    {IRNodeType::JNE, "ne"}
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