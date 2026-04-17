#ifndef OPTIMIZATION_CONSTANT_FOLDING_HPP
#define OPTIMIZATION_CONSTANT_FOLDING_HPP

#include <string>
#include <format>
#include <utility>
#include <memory>

#include "../common/defs/types.hpp"
#include "../common/abstract-syntax-tree/ast_binary_expr.hpp"
#include "../common/intermediate-representation-tree/ir_expr.hpp"
#include "../common/intermediate-representation-tree/ir_literal_expr.hpp"

/** 
 * @namespace optimization::constant_folding
 * @brief Module for computing the operations that can be resolved in a compile-time
*/
namespace optimization::constant_folding {
    /** 
     * @struct MergeResult
     * @brief result of the merge operation
    */
    template<typename T>
    struct MergeResult {
        T result;
        std::string error{""};
    };

    /** 
     * @brief performs operation on two constants
     * @note T - type of the constants
     * @param l - left operand
     * @param r - right operand
     * @param op - operator
     * @param line - line of the token
     * @param column - column of the token
     * @returns result of the merge operation
    */
    template<typename T>
    MergeResult<T> mergeValues(T l, T r, Operator op, size_t line, size_t column){
        MergeResult<T> constFold;
        switch(op){
            case Operator::ADD:
                constFold.result = l + r;
                return constFold;
            case Operator::SUB:
                constFold.result = l - r;
                return constFold;
            case Operator::MUL:
                constFold.result = l * r;
                return constFold;
            case Operator::DIV:
                if(r == 0){
                    constFold.result = 0;
                    constFold.error = std::format(
                        "Line {}, Column {}: SEMANTIC ERROR -> division by ZERO", 
                        line, column
                    );
                    return constFold;
                }
                constFold.result = l / r;
                return constFold;
            case Operator::ANDB:
                constFold.result = l & r;
                return constFold;
            case Operator::ORB:
                constFold.result = l | r;
                return constFold;
            case Operator::XOR:
                constFold.result = l ^ r;
                return constFold;
            case Operator::LSHIFT:
                constFold.result = l << r;
                return constFold;
            case Operator::RSHIFT:
                constFold.result = l >> r;
                return constFold;
            case Operator::ANDL:
                constFold.result = l && r;
                return constFold;
            case Operator::ORL:
                constFold.result = l || r;
                return constFold;
            case Operator::GREATER:
                constFold.result = static_cast<T>(l > r);
                return constFold;
            case Operator::LESS:
                constFold.result = static_cast<T>(l < r);
                return constFold;
            case Operator::GEQUAL:
                constFold.result = static_cast<T>(l >= r);
                return constFold;
            case Operator::LEQUAL:
                constFold.result = static_cast<T>(l <= r);
                return constFold;
            case Operator::EQUAL:
                constFold.result = static_cast<T>(l == r);
                return constFold;
            case Operator::NEQUAL:
                constFold.result = static_cast<T>(l != r);
                return constFold;
            default:
                std::unreachable();
        }
    }

    /**
     * @brief helper function to get operand value
     * @returns operand value as T
    */
    template<typename T>
    T getOperandValue(const ir::IRLiteralExpr* operand) {
        if (std::is_same<T, int>::value) {
            return static_cast<T>(std::stoi(operand->getValue()));
        } else {
            return static_cast<T>(std::stoul(operand->getValue()));
        }
    }
    
    /**
     * @brief merges literals of the expression
     * @note reduces the depth of the expression subtree when all children are literals
     * @param leftOperand - const pointer to the irt literal expression
     * @param rightOperand - const pointer to the irt literal expression
     * @param binExp - const pointer to the ast binary expression, contains operation
     * @returns result of the merge operation
    */
    template<typename T>
    MergeResult<std::unique_ptr<ir::IRExpr>> mergeLiterals(
        const ir::IRLiteralExpr* leftOperand, 
        const ir::IRLiteralExpr* rightOperand, 
        const syntax::ast::ASTBinaryExpr* binExp
    ){
        T lval{ getOperandValue<T>(leftOperand) };
        T rval{ getOperandValue<T>(rightOperand) };

        const auto& binExpToken{ binExp->getToken() };

        MergeResult<T> res { mergeValues<T>(
            lval, 
            rval, 
            binExp->getOperator(), 
            binExpToken.line, 
            binExpToken.column
        ) };

        Type type{ binExp->getType() };
        std::string suffix{ type == Type::UNSIGNED ? "u" : "" };

        MergeResult<std::unique_ptr<ir::IRExpr>> foldedExpr {
            .result = std::make_unique<ir::IRLiteralExpr>(std::to_string(res.result) + suffix, type),
            .error = res.error
        };

        return foldedExpr;
    }

};

#endif