#ifndef IRTREE_DEFS_HPP
#define IRTREE_DEFS_HPP

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>

#include "../../defs/defs.hpp"

/*
    intermediate representation related definitions
*/

enum class IRNodeType{
    PROGRAM, FUNCTION, PARAMETER, VARIABLE, ARGUMENT, ID, LITERAL, IF, WHILE, FOR, DO_WHILE, SWITCH, CASE, SWITCH_BLOCK,
    DEFAULT, BREAK, ASSIGN, COMPOUND, CALL, RETURN, TEMPORARY, PRINTF, ADD, SUB, DIV, MUL, CMP, AND, OR, XOR, SHL, SAL, SHR, SAR,
    JG, JA, JL, JB, JGE, JAE, JLE, JBE, JE, JNE, NONE
};

struct Operation {
    IRNodeType _int;
    IRNodeType _unsigned;

    Operation(IRNodeType i, IRNodeType uns) : _int{ i }, _unsigned{ uns }{}
    IRNodeType getOperation(Types type) const noexcept {
        switch(type){
            case Types::INT:
                return _int;
            case Types::UNSIGNED:
                return _unsigned;
            default:
                //unreachable
                return IRNodeType::NONE;
        }
    }
};

extern const std::unordered_set<IRNodeType> irOperators;

extern const std::unordered_map<IRNodeType, std::string> irNodeToString;

extern const std::unordered_map<std::string, IRNodeType> stringToArop;

extern const std::unordered_map<std::string, std::vector<IRNodeType>> stringToBitop;

extern const std::unordered_map<Operators, Operation> operatorToIRNodeType;

#endif
