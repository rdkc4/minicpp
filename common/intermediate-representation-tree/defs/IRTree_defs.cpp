#include "IRTree_defs.hpp"

const std::unordered_set<IRNodeType> irOperators {IRNodeType::ADD, IRNodeType::SUB, IRNodeType::DIV, IRNodeType::MUL, 
    IRNodeType::AND, IRNodeType::OR, IRNodeType::XOR, IRNodeType::SHL, IRNodeType::SAL, IRNodeType::SHR, IRNodeType::SAR
};

const std::unordered_map<IRNodeType, std::string> irNodeToString {
    {IRNodeType::PROGRAM, "PROGRAM"}, {IRNodeType::FUNCTION, "FUNCTION"},
    {IRNodeType::PARAMETER, "PARAMETER"},
    {IRNodeType::VARIABLE, "VARIABLE"},
    {IRNodeType::ARGUMENT, "ARGUMENT"},
    {IRNodeType::ID, "ID"},
    {IRNodeType::LITERAL, "LITERAL"},
    {IRNodeType::IF, "IF"},
    {IRNodeType::WHILE, "WHILE"},
    {IRNodeType::FOR, "FOR"},
    {IRNodeType::DO_WHILE, "DO_WHILE"},
    {IRNodeType::SWITCH, "SWITCH"},
    {IRNodeType::CASE, "CASE"},
    {IRNodeType::DEFAULT, "DEFAULT"},
    {IRNodeType::SWITCH_BLOCK, "SW_BLOCK"},
    {IRNodeType::BREAK, "BREAK"},
    {IRNodeType::ASSIGN, "ASSIGN"},
    {IRNodeType::COMPOUND, "COMPOUND"},
    {IRNodeType::CALL, "CALL"},
    {IRNodeType::RETURN, "RETURN"},
    {IRNodeType::ASSIGN, "ASSIGN"},
    {IRNodeType::ADD, "add"},
    {IRNodeType::SUB, "sub"},
    {IRNodeType::MUL, "mul"},
    {IRNodeType::DIV, "div"},
    {IRNodeType::CMP, "cmp"},
    {IRNodeType::AND, "and"},
    {IRNodeType::OR, "or"},
    {IRNodeType::XOR, "xor"},
    {IRNodeType::SHL, "shl"},
    {IRNodeType::SAL, "sal"},
    {IRNodeType::SHR, "shr"},
    {IRNodeType::SAR, "sar"},
    {IRNodeType::JG, "jg"},
    {IRNodeType::JA, "ja"},
    {IRNodeType::JL, "jl"},
    {IRNodeType::JB, "jb"},
    {IRNodeType::JGE, "jge"},
    {IRNodeType::JAE, "jae"},
    {IRNodeType::JLE, "jle"},
    {IRNodeType::JBE, "jbe"},
    {IRNodeType::JE, "je"},
    {IRNodeType::JNE, "jne"},
    {IRNodeType::TEMPORARY, "TMP"},
    {IRNodeType::PRINTF, "printf"}
};

const std::unordered_map<std::string, IRNodeType> stringToArop {
    {"+", IRNodeType::ADD},
    {"-", IRNodeType::SUB},
    {"*", IRNodeType::MUL},
    {"/", IRNodeType::DIV}
};

const std::unordered_map<std::string, std::vector<IRNodeType>> stringToBitop {
    {"&", {IRNodeType::AND, IRNodeType::AND}},
    {"|", {IRNodeType::OR, IRNodeType::OR}},
    {"^", {IRNodeType::XOR, IRNodeType::XOR}},
    {"<<", {IRNodeType::SAL, IRNodeType::SHL}},
    {">>", {IRNodeType::SAR, IRNodeType::SHR}}
};

const std::unordered_map<Operators, Operation> operatorToIRNodeType {
    {Operators::ADD, {IRNodeType::ADD, IRNodeType::ADD}},
    {Operators::SUB, {IRNodeType::SUB, IRNodeType::SUB}},
    {Operators::MUL, {IRNodeType::MUL, IRNodeType::MUL}},
    {Operators::DIV, {IRNodeType::DIV, IRNodeType::DIV}},
    {Operators::ANDB, {IRNodeType::AND, IRNodeType::AND}},
    {Operators::ORB, {IRNodeType::OR, IRNodeType::OR}},
    {Operators::XOR, {IRNodeType::XOR, IRNodeType::XOR}},
    {Operators::LSHIFT, {IRNodeType::SAL, IRNodeType::SHL}},
    {Operators::RSHIFT, {IRNodeType::SAR, IRNodeType::SHR}},
    {Operators::GREATER, {IRNodeType::JG, IRNodeType::JA}},
    {Operators::LESS, {IRNodeType::JL, IRNodeType::JB}},
    {Operators::GEQUAL, {IRNodeType::JGE, IRNodeType::JAE}},
    {Operators::LEQUAL, {IRNodeType::JLE, IRNodeType::JBE}},
    {Operators::EQUAL, {IRNodeType::JE, IRNodeType::JE}},
    {Operators::NEQUAL, {IRNodeType::JNE, IRNodeType::JNE}}
};