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
enum class IRNodeType {
    /// program structure
    PROGRAM,         //< root of ir
    FUNCTION,        //< function definition
    PARAMETER,       //< function parameter

    /// statements
    VARIABLE,        //< variable declaration

    /// control flow statements
    IF,
    WHILE, 
    DO_WHILE, 
    FOR, 
    COMPOUND, 
    ASSIGN, 
    RETURN, 
    CALL_STMT, 
    SWITCH, 
    CASE, 
    SWITCH_BLOCK,
    DEFAULT, 
    BREAK, 

    /// values
    TEMPORARY,       //< compiler-generated temporaries
    CALL,            //< function call returning value
    ARGUMENT,        //< function call argument
    ID,              //< identifier reference
    LITERAL,         //< constant value

    /// arithmetic operations
    ADD, SUB, DIV, MUL, 
    
    /// bitwise operations
    AND, OR, XOR, SHL, SAL, SHR, SAR,
    
    /// comparison
    CMP, 

    /// conditional jumps
    JG, JA,          //< greater (signed / unsigned)
    JL, JB,          //< less (signed, unsigned)
    JGE, JAE,        //< greater or equal (signed, unsigned)
    JLE, JBE,        //< less or equal (signed, unsigned)
    JE, JNE,         //< equal / not equal
    
    /// special 
    NONE 
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

    /**
     * @brief creates the instance of the operation
     * @param i - operation for signed integers
     * @param uns - operation for unsigned integers
    */
    Operation(IRNodeType i, IRNodeType uns) : _int{ i }, _unsigned{ uns }{}

    /** 
     * @brief getter for the operation based on a type it is used on
     * @param type - type of the operands
     * @returns irt node type specialized for the type
    */
    constexpr IRNodeType getOperation(Type type) const noexcept {
        switch(type){
            case Type::INT:
                return _int;
            case Type::UNSIGNED:
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
extern const std::unordered_map<Operator, Operation> operatorToIRNodeType;

#endif
