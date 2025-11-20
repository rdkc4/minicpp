#ifndef IRTREE_DEFS_HPP
#define IRTREE_DEFS_HPP

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <cassert>

#include "../../defs/defs.hpp"

/** 
 * @enum IRNodeType
 * @brief types of the irt nodes
*/
enum class IRNodeType{
    PROGRAM, FUNCTION, PARAMETER, VARIABLE, ARGUMENT, ID, LITERAL, IF, WHILE, FOR, DO_WHILE, SWITCH, CASE, SWITCH_BLOCK,
    DEFAULT, BREAK, ASSIGN, COMPOUND, CALL, RETURN, TEMPORARY, PRINTF, ADD, SUB, DIV, MUL, CMP, AND, OR, XOR, SHL, SAL, SHR, SAR,
    JG, JA, JL, JB, JGE, JAE, JLE, JBE, JE, JNE, NONE
};

/** 
 * @struct Operation
 * @brief maps type to the operation specialized for the type
*/
struct Operation {
    /// irt node type specified for int 
    IRNodeType _int;
    /// irt node type specified for unsigned
    IRNodeType _unsigned;

    Operation(IRNodeType i, IRNodeType uns) : _int{ i }, _unsigned{ uns }{}

    /** 
     * @brief getter for the operation based on a type it is used on
     * @param type - type of the operands
     * @returns irt node type specialized for the type
    */
    constexpr IRNodeType getOperation(Types type) const noexcept {
        switch(type){
            case Types::INT:
                return _int;
            case Types::UNSIGNED:
                return _unsigned;
            default:
                return IRNodeType::NONE;
        }
    }
};

/// set of all irt operators
extern const std::unordered_set<IRNodeType> irOperators;

/// translates irt node types to string
extern const std::unordered_map<IRNodeType, std::string> irNodeToString;

/// translates strings to arithmetic operator nodes
extern const std::unordered_map<std::string, IRNodeType> stringToArop;

/// translates strings to bitwise operators
extern const std::unordered_map<std::string, std::vector<IRNodeType>> stringToBitop;

/// maps the operators to the operation
extern const std::unordered_map<Operators, Operation> operatorToIRNodeType;

#endif
