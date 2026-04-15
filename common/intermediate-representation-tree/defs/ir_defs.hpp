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
    constexpr size_t IR_NODE_TYPE_COUNT{ static_cast<size_t>(IRNodeType::NONE) + 1 };

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

    /// maps the ir node types to their string representation
    constexpr std::array<std::string_view, IR_NODE_TYPE_COUNT> irNodeTypeStringRepresentations {
        [] {
            /**
             * @brief converts ir node type enum to array index
             * @param type - element of the ir node type enum
             * @returns index in the array for provided node type
            */
            constexpr auto idx {
                [](IRNodeType type) noexcept -> size_t {
                    return static_cast<size_t>(type);
                }
            };

            std::array<std::string_view, IR_NODE_TYPE_COUNT> str;

            str[idx(IRNodeType::PROGRAM)]       = "PROGRAM";
            str[idx(IRNodeType::FUNCTION)]      = "FUNCTION";
            str[idx(IRNodeType::PARAMETER)]     = "PARAMETER";
            str[idx(IRNodeType::VARIABLE)]      = "VARIABLE";

            str[idx(IRNodeType::IF)]            = "IF";
            str[idx(IRNodeType::WHILE)]         = "WHILE";
            str[idx(IRNodeType::DO_WHILE)]      = "DO_WHILE";
            str[idx(IRNodeType::FOR)]           = "FOR";

            str[idx(IRNodeType::COMPOUND)]      = "COMPOUND";
            str[idx(IRNodeType::ASSIGN)]        = "ASSIGN";
            str[idx(IRNodeType::RETURN)]        = "RETURN";
            str[idx(IRNodeType::CALL_STMT)]     = "CALL_STMT";

            str[idx(IRNodeType::SWITCH)]        = "SWITCH";
            str[idx(IRNodeType::CASE)]          = "CASE";
            str[idx(IRNodeType::SWITCH_BLOCK)]  = "SWITCH_BLOCK";
            str[idx(IRNodeType::DEFAULT)]       = "DEFAULT";
            str[idx(IRNodeType::BREAK)]         = "BREAK";

            str[idx(IRNodeType::TEMPORARY)]     = "TEMPORARY";
            str[idx(IRNodeType::CALL)]          = "CALL";
            str[idx(IRNodeType::ARGUMENT)]      = "ARGUMENT";
            str[idx(IRNodeType::ID)]            = "ID";
            str[idx(IRNodeType::LITERAL)]       = "LITERAL";

            str[idx(IRNodeType::ADD)]           = "add";
            str[idx(IRNodeType::SUB)]           = "sub";
            str[idx(IRNodeType::MUL)]           = "mul";
            str[idx(IRNodeType::DIV)]           = "div";

            str[idx(IRNodeType::AND)]           = "and";
            str[idx(IRNodeType::OR)]            = "or";
            str[idx(IRNodeType::XOR)]           = "xor";
            str[idx(IRNodeType::SHL)]           = "shl";
            str[idx(IRNodeType::SAL)]           = "sal";
            str[idx(IRNodeType::SHR)]           = "shr";
            str[idx(IRNodeType::SAR)]           = "sar";

            str[idx(IRNodeType::ANDL)]          = "andl";
            str[idx(IRNodeType::ORL)]           = "orl";

            str[idx(IRNodeType::CMP)]           = "cmp";

            str[idx(IRNodeType::JG)]            = "jg";
            str[idx(IRNodeType::JA)]            = "ja";
            str[idx(IRNodeType::JL)]            = "jl";
            str[idx(IRNodeType::JB)]            = "jb";
            str[idx(IRNodeType::JGE)]           = "jge";
            str[idx(IRNodeType::JAE)]           = "jae";
            str[idx(IRNodeType::JLE)]           = "jle";
            str[idx(IRNodeType::JBE)]           = "jbe";
            str[idx(IRNodeType::JE)]            = "je";
            str[idx(IRNodeType::JNE)]           = "jne";

            str[idx(IRNodeType::NONE)]          = "NONE";

            return str;
        }()
    };

    /**
     * @brief getter for the string representation of the ir node
     * @param nodeType - ir node type
     * @returns string representation of the node
    */
    constexpr std::string_view irNodeToStr(IRNodeType nodeType) {
        return irNodeTypeStringRepresentations[static_cast<size_t>(nodeType)];
    }

    /// maps operator to ir instruction node
    constexpr std::array<Operation, OPERATOR_COUNT> operationTable {
        [] {
            /**
             * @brief converts operator enum to array index
             * @param op - element of the operator enum
             * @returns index in the array for provided operator
            */
            constexpr auto idx {
                [](Operator op) noexcept -> size_t {
                    return static_cast<size_t>(op);
                }
            };

            std::array<Operation, OPERATOR_COUNT> ops{};

            ops[idx(Operator::NO_OP)]   = { .intOp = IRNodeType::NONE, .unsignedOp = IRNodeType::NONE };

            ops[idx(Operator::ADD)]     = { .intOp = IRNodeType::ADD,  .unsignedOp = IRNodeType::ADD };
            ops[idx(Operator::SUB)]     = { .intOp = IRNodeType::SUB,  .unsignedOp = IRNodeType::SUB };
            ops[idx(Operator::MUL)]     = { .intOp = IRNodeType::MUL,  .unsignedOp = IRNodeType::MUL };
            ops[idx(Operator::DIV)]     = { .intOp = IRNodeType::DIV,  .unsignedOp = IRNodeType::DIV };

            ops[idx(Operator::ANDB)]    = { .intOp = IRNodeType::AND,  .unsignedOp = IRNodeType::AND };
            ops[idx(Operator::ORB)]     = { .intOp = IRNodeType::OR,   .unsignedOp = IRNodeType::OR };
            ops[idx(Operator::XOR)]     = { .intOp = IRNodeType::XOR,  .unsignedOp = IRNodeType::XOR };
            ops[idx(Operator::LSHIFT)]  = { .intOp = IRNodeType::SAL,  .unsignedOp = IRNodeType::SHL };
            ops[idx(Operator::RSHIFT)]  = { .intOp = IRNodeType::SAR,  .unsignedOp = IRNodeType::SHR };

            ops[idx(Operator::ANDL)]    = { .intOp = IRNodeType::ANDL, .unsignedOp = IRNodeType::ANDL };
            ops[idx(Operator::ORL)]     = { .intOp = IRNodeType::ORL,  .unsignedOp = IRNodeType::ORL };

            ops[idx(Operator::LESS)]    = { .intOp = IRNodeType::JL,   .unsignedOp = IRNodeType::JB };
            ops[idx(Operator::GREATER)] = { .intOp = IRNodeType::JG,   .unsignedOp = IRNodeType::JA };
            ops[idx(Operator::LEQUAL)]  = { .intOp = IRNodeType::JLE,  .unsignedOp = IRNodeType::JBE };
            ops[idx(Operator::GEQUAL)]  = { .intOp = IRNodeType::JGE,  .unsignedOp = IRNodeType::JAE };
            ops[idx(Operator::EQUAL)]   = { .intOp = IRNodeType::JE,   .unsignedOp = IRNodeType::JE };
            ops[idx(Operator::NEQUAL)]  = { .intOp = IRNodeType::JNE,  .unsignedOp = IRNodeType::JNE };

            return ops;
        }()
    };

    /**
     * @brief getter for the node that represents operation
     * @param op - operator
     * @param type - type of operands
     * @returns node type of the operation
    */
    constexpr IRNodeType resolveOperator(Operator op, Type type) {
        return operationTable[static_cast<size_t>(op)].get(type);
    }

    static_assert(irNodeTypeStringRepresentations.size() == IR_NODE_TYPE_COUNT);
    static_assert(operationTable.size() == OPERATOR_COUNT);
}

#endif
