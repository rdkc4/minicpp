#include "ir_defs.hpp"

const std::unordered_map<IRNodeType, std::string> irNodeToString {
    /// program structure
    {IRNodeType::PROGRAM, "PROGRAM"}, 
    {IRNodeType::FUNCTION, "FUNCTION"},
    {IRNodeType::PARAMETER, "PARAMETER"},

    /// statements
    {IRNodeType::VARIABLE, "VARIABLE"},
    {IRNodeType::ASSIGN, "ASSIGN"},

    /// control flow statements
    {IRNodeType::IF, "IF"},
    {IRNodeType::WHILE, "WHILE"},
    {IRNodeType::FOR, "FOR"},
    {IRNodeType::DO_WHILE, "DO_WHILE"},
    {IRNodeType::SWITCH, "SWITCH"},
    {IRNodeType::CASE, "CASE"},
    {IRNodeType::DEFAULT, "DEFAULT"},
    {IRNodeType::SWITCH_BLOCK, "SW_BLOCK"},
    {IRNodeType::BREAK, "BREAK"},
    {IRNodeType::COMPOUND, "COMPOUND"},
    {IRNodeType::CALL_STMT, "CALL_STMT"},
    {IRNodeType::RETURN, "RETURN"},

    /// values
    {IRNodeType::ID, "ID"},
    {IRNodeType::LITERAL, "LITERAL"},
    {IRNodeType::CALL, "CALL"},
    {IRNodeType::ARGUMENT, "ARGUMENT"},
    {IRNodeType::TEMPORARY, "TMP"},

    /// arithmetic operators
    {IRNodeType::ADD, "add"},
    {IRNodeType::SUB, "sub"},
    {IRNodeType::MUL, "mul"},
    {IRNodeType::DIV, "div"},

    /// bitwise operators
    {IRNodeType::AND, "and"},
    {IRNodeType::OR, "or"},
    {IRNodeType::XOR, "xor"},
    {IRNodeType::SHL, "shl"},
    {IRNodeType::SAL, "sal"},
    {IRNodeType::SHR, "shr"},
    {IRNodeType::SAR, "sar"},

    /// logical operators
    {IRNodeType::ANDL, "andl"},
    {IRNodeType::ORL, "orl"},
    
    /// comparison
    {IRNodeType::CMP, "cmp"},

    /// jumps
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

const std::unordered_map<Operator, Operation> operatorToIRNodeType {
    {Operator::ADD, {IRNodeType::ADD, IRNodeType::ADD}},
    {Operator::SUB, {IRNodeType::SUB, IRNodeType::SUB}},
    {Operator::MUL, {IRNodeType::MUL, IRNodeType::MUL}},
    {Operator::DIV, {IRNodeType::DIV, IRNodeType::DIV}},
    {Operator::ANDB, {IRNodeType::AND, IRNodeType::AND}},
    {Operator::ORB, {IRNodeType::OR, IRNodeType::OR}},
    {Operator::XOR, {IRNodeType::XOR, IRNodeType::XOR}},
    {Operator::LSHIFT, {IRNodeType::SAL, IRNodeType::SHL}},
    {Operator::RSHIFT, {IRNodeType::SAR, IRNodeType::SHR}},
    {Operator::ANDL, {IRNodeType::ANDL, IRNodeType::ANDL}},
    {Operator::ORL, {IRNodeType::ORL, IRNodeType::ORL}},
    {Operator::GREATER, {IRNodeType::JG, IRNodeType::JA}},
    {Operator::LESS, {IRNodeType::JL, IRNodeType::JB}},
    {Operator::GEQUAL, {IRNodeType::JGE, IRNodeType::JAE}},
    {Operator::LEQUAL, {IRNodeType::JLE, IRNodeType::JBE}},
    {Operator::EQUAL, {IRNodeType::JE, IRNodeType::JE}},
    {Operator::NEQUAL, {IRNodeType::JNE, IRNodeType::JNE}}
};