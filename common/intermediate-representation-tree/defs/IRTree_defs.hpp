#ifndef IRTREE_DEFS_HPP
#define IRTREE_DEFS_HPP

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>

/*
    intermediate representation related definitions
*/

enum class IRNodeType{
    PROGRAM, FUNCTION, PARAMETER, VARIABLE, ARGUMENT, ID, LITERAL, IF, WHILE, FOR, DO_WHILE, SWITCH, CASE, 
    DEFAULT, BREAK, ASSIGN, COMPOUND, CALL, RETURN, ADD, SUB, DIV, MUL, CMP, AND, OR, XOR, SHL, SAL, SHR, SAR, TEMPORARY
};

extern const std::unordered_set<IRNodeType> irOperators;

extern const std::unordered_map<IRNodeType, std::string> irNodeToString;

extern const std::unordered_map<std::string, IRNodeType> stringToArop;

extern const std::unordered_map<std::string, std::vector<IRNodeType>> stringToBitop;

#endif
