#ifndef OPTIMIZATION_CONSTANT_FOLDING_HPP
#define OPTIMIZATION_CONSTANT_FOLDING_HPP

#include <string>
#include <format>
#include <utility>

#include "../common/defs/defs.hpp"

namespace Optimization {

    /** 
     * @namespace ConstantFolding
     * @brief Module for computing the operations that can be resolved in a compile-time
    */
    namespace ConstantFolding {

        /** 
         * @struct MergeResult
         * @brief result of the merge operation
        */
        template<typename T>
        struct MergeResult {
            T result;
            std::string error = "";
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
                        constFold.error = std::format("Line {}, Column {}: SEMANTIC ERROR -> division by ZERO", line, column);
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
    }
};

#endif