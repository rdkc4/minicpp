#ifndef IRTREE_DEFS_HPP
#define IRTREE_DEFS_HPP

#include <array>
#include <string_view>
#include <cassert>

#include "../../defs/defs.hpp"

/**
 * @namespace IR::defs
 * @brief module defining the definitions of the intermediate representation
*/
namespace IR::defs {
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
        ADD, SUB, MUL, DIV, 
        
        /// bitwise operations
        AND, OR, XOR, SHL, SAL, SHR, SAR,

        /// logical operations
        ANDL, ORL,

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

    /// number of ir node types
    constexpr size_t IR_NODE_TYPE_COUNT = static_cast<size_t>(IRNodeType::NONE) + 1;

    /** 
    * @struct Operation
    * @brief maps type to the operation specialized for the type
    */
    struct Operation {
        /// irt node type specified for int 
        IRNodeType intOp;

        /// irt node type specified for unsigned
        IRNodeType unsignedOp;

        /**
         * @brief getter for the node that represents specific type
         * @param type - type of the operands
         * @returns node type of the operation
        */
        constexpr IRNodeType get(Type type) const {
            return (type == Type::INT) ? intOp : unsignedOp;
        }

    };

    constexpr std::array<std::string_view, IR_NODE_TYPE_COUNT> nodeToStr {
        "PROGRAM",       //< IRNodeType::PROGRAM
        "FUNCTION",      //< IRNodeType::FUNCTION
        "PARAMETER",     //< IRNodeType::PARAMETER
        "VARIABLE",      //< IRNodeType::VARIABLE
        "IF",            //< IRNodeType::IF
        "WHILE",         //< IRNodeType::WHILE
        "DO_WHILE",      //< IRNodeType::DO_WHILE
        "FOR",           //< IRNodeType::FOR
        "COMPOUND",      //< IRNodeType::COMPOUND
        "ASSIGN",        //< IRNodeType::ASSIGN 
        "RETURN",        //< IRNodeType::RETURN 
        "CALL_STMT",     //< IRNodeType::CALL_STMT
        "SWITCH",        //< IRNodeType::SWITCH
        "CASE",          //< IRNodeType::CASE
        "SW_BLOCK",      //< IRNodeType::SWITCH_BLOCK
        "DEFAULT",       //< IRNodeType::DEFAULT
        "BREAK",         //< IRNodeType::BREAK
        "TMP",           //< IRNodeType::TEMPORARY
        "CALL",          //< IRNodeType::CALL
        "ARGUMENT",      //< IRNodeType::ARGUMENT
        "ID",            //< IRNodeType::ID
        "LITERAL",       //< IRNodeType::LITERAL
        "add",           //< IRNodeType::ADD
        "sub",           //< IRNodeType::SUB
        "mul",           //< IRNodeType::MUL
        "div",           //< IRNodeType::DIV
        "and",           //< IRNodeType::AND
        "or",            //< IRNodeType::OR
        "xor",           //< IRNodeType::XOR 
        "shl",           //< IRNodeType::SHL 
        "sal",           //< IRNodeType::SAL 
        "shr",           //< IRNodeType::SHR 
        "sar",           //< IRNodeType::SAR 
        "andl",          //< IRNodeType::ANDL
        "orl",           //< IRNodeType::ORL 
        "cmp",           //< IRNodeType::CMP 
        "jg",            //< IRNodeType::JG
        "ja",            //< IRNodeType::JA
        "jl",            //< IRNodeType::JL
        "jb",            //< IRNodeType::JB
        "jge",           //< IRNodeType::JGE
        "jae",           //< IRNodeType::JAE
        "jle",           //< IRNodeType::JLE
        "jbe",           //< IRNodeType::JBE
        "je",            //< IRNodeType::JE
        "jne",           //< IRNodeType::JNE
        "NONE",          //< IRNodeType::NONE
    };

    /// maps the operators to the operation
    constexpr std::array<Operation, OPERATOR_COUNT> operationTable {
        Operation{.intOp = IRNodeType::NONE, .unsignedOp = IRNodeType::NONE},    //< Operator::NO_OP
        Operation{.intOp = IRNodeType::ADD, .unsignedOp = IRNodeType::ADD},      //< Operator::ADD
        Operation{.intOp = IRNodeType::SUB, .unsignedOp = IRNodeType::SUB},      //< Operator::SUB
        Operation{.intOp = IRNodeType::MUL, .unsignedOp = IRNodeType::MUL},      //< Operator::MUL
        Operation{.intOp = IRNodeType::DIV, .unsignedOp = IRNodeType::DIV},      //< Operator::DIV
        Operation{.intOp = IRNodeType::AND, .unsignedOp = IRNodeType::AND},      //< Operator::ANDB
        Operation{.intOp = IRNodeType::OR, .unsignedOp = IRNodeType::OR},        //< Operator::ORB
        Operation{.intOp = IRNodeType::XOR, .unsignedOp = IRNodeType::XOR},      //< Operator::XOR
        Operation{.intOp = IRNodeType::SAL, .unsignedOp = IRNodeType::SHL},      //< Operator::LSHIFT
        Operation{.intOp = IRNodeType::SAR, .unsignedOp = IRNodeType::SHR},      //< Operator::RSHIFT
        Operation{.intOp = IRNodeType::ANDL, .unsignedOp = IRNodeType::ANDL},  //< Operator::ANDL
        Operation{.intOp = IRNodeType::ORL, .unsignedOp = IRNodeType::ORL},    //< Operator::ORL
        Operation{.intOp = IRNodeType::JL, .unsignedOp = IRNodeType::JB},      //< Operator::LESS
        Operation{.intOp = IRNodeType::JG, .unsignedOp = IRNodeType::JA},      //< Operator::GREATER
        Operation{.intOp = IRNodeType::JLE, .unsignedOp = IRNodeType::JBE},    //< Operator::LEQAUL
        Operation{.intOp = IRNodeType::JGE, .unsignedOp = IRNodeType::JAE},    //< Operator::GEQUAL
        Operation{.intOp = IRNodeType::JE, .unsignedOp = IRNodeType::JE},      //< Operator::EQUAL
        Operation{.intOp = IRNodeType::JNE, .unsignedOp = IRNodeType::JNE}     //< Operator::NEQUAL
    };

    /**
     * @brief getter for the node that represents operation
     * @param op - operator
     * @param type - type of operands
     * @returns node type of the operation
    */
    constexpr IRNodeType resolveOperator(Operator op, Type type) {
        assert(type == Type::INT || type == Type::UNSIGNED);
        assert(static_cast<size_t>(op) < OPERATOR_COUNT);

        return operationTable[static_cast<size_t>(op)].get(type);
    }

}

#endif
