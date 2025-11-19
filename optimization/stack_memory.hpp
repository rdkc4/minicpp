#ifndef OPTIMIZATION_STACK_MEMORY_HPP
#define OPTIMIZATION_STACK_MEMORY_HPP

#include "../common/intermediate-representation-tree/IRFunction.hpp"
#include "../common/intermediate-representation-tree/IRVariable.hpp"
#include "../common/intermediate-representation-tree/IRCompoundSt.hpp"
#include "../common/intermediate-representation-tree/IRIfSt.hpp"
#include "../common/intermediate-representation-tree/IRForSt.hpp"
#include "../common/intermediate-representation-tree/IRWhileSt.hpp"
#include "../common/intermediate-representation-tree/IRDoWhileSt.hpp"
#include "../common/intermediate-representation-tree/IRPrintfSt.hpp"
#include "../common/intermediate-representation-tree/IRAssignSt.hpp"
#include "../common/intermediate-representation-tree/IRReturnSt.hpp"
#include "../common/intermediate-representation-tree/IRSwitchSt.hpp"
#include "../common/intermediate-representation-tree/IRFunctionCall.hpp"

namespace Optimization {

    /** 
     * @namespace StackMemory
     * @brief Module for calculating the required memory for variables on the stack
    */
    namespace StackMemory {
        /// size of the register in bytes
        constexpr size_t regSize = 8;

        /** 
         * @brief calculates the memory required for the stack of the function
         * @param _function - const pointer to the irt function
         * @returns total number of bytes required for the stack of the function for local variables
        */
        size_t computeStackMemory(const IRFunction* _function);

        /** 
         * @brief calculates the number of temporary variables
         * @param _temporary - const pointer to the irt temporary
         * @returns number of temporary variables
        */
        size_t computeStackMemory(const IRTemporary* _temporary);

        /** 
         * @brief calculates the number of local variables in a statement
         * @param _stmt - const pointer to the irt statement
         * @returns number of local variables in a statement
        */
        size_t computeStackMemory(const IRStatement* _stmt);

        /** 
         * @brief calculates the number of local variables in a variable declaration
         * @param _variable - const pointer to the irt variable
         * @returns number of variables in a variable declaration
         * @note initial variable + number of temporary variables
        */
        size_t computeStackMemory(const IRVariable* _variable);

        /** 
         * @brief calculates the number of local variables in a compound statement
         * @param _compound - const pointer to irt compound statement
         * @returns number of local variables in a compound statement
        */
        size_t computeStackMemory(const IRCompoundSt* _compound);

        /** 
         * @brief calculates the number of local variables in an if-statement
         * @param _if - const pointer to the irt if-statement
         * @returns number of local variables in an if-statement
        */
        size_t computeStackMemory(const IRIfSt* _if);

        /** 
         * @brief calculates the number of local variables in a for-statement
         * @param _for - const pointer to the irt for-statement
         * @returns number of local variables in a for-statement
        */
        size_t computeStackMemory(const IRForSt* _for);

        /** 
         * @brief calculates the number of local variables in a while-statement
         * @param _while - const pointer to the irt while-statement
         * @returns number of local variables in a while-statement
        */
        size_t computeStackMemory(const IRWhileSt* _while);

        /** 
         * @brief calculates the number of local variables in a do-while statement
         * @param _dowhile - const pointer to the irt do-while statement
         * @returns number of local variables in a do-while statement
        */
        size_t computeStackMemory(const IRDoWhileSt* _dowhile);

        /** 
         * @brief calculates the number of temporary variables in a printf statement
         * @param _printf - const pointer to the irt printf statement
         * @returns number of temporary variables in a printf statement
        */
        size_t computeStackMemory(const IRPrintfSt* _printf);

        /** 
         * @brief calculates the number of temporary variables in an assignment statement
         * @param _assign - const pointer to the irt assignment statement
         * @returns number of temporary variables in an assignment statement
        */
        size_t computeStackMemory(const IRAssignSt* _assign);

        /** 
         * @brief calculates the number of temporary variables in a return statement
         * @param _return - const pointer to the irt return statement
         * @returns number of temporary variables in a return statement
        */
        size_t computeStackMemory(const IRReturnSt* _return);

        /** 
         * @brief calculates the number of local variables in a switch-statement
         * @param _switch - const pointer to the irt switch-statement
         * @returns number of local variables in a switch-statement
        */
        size_t computeStackMemory(const IRSwitchSt* _switch);

        /** 
         * @brief calculates the number of local variables in a switch-block
         * @param _swBlock - const pointer to the irt switch-block
         * @returns number of local variables in a switch-block
        */
        size_t computeStackMemory(const IRSwitchBlock* _swBlock);

        /** 
         * @brief calculates the number of temporary variables in a function call
         * @param _functionCall - const pointer to the irt function call
         * @returns number of temporary variables in a function call
        */
        size_t computeStackMemory(const IRFunctionCall* _functionCall);

        /** 
         * @brief overload for the nodes that don't need any space on the stack
         * @param _node - const pointer to irt node (unused)
         * @returns zero
        */
        size_t computeStackMemory([[maybe_unused]] const IRNode* _node);
    };
};

#endif