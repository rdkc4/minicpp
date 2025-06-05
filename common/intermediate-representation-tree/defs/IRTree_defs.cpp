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
    {IRNodeType::SAR, "sar"}
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