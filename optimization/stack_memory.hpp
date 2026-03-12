#ifndef OPTIMIZATION_STACK_MEMORY_HPP
#define OPTIMIZATION_STACK_MEMORY_HPP

#include "../common/intermediate-representation-tree/ir_function.hpp"
#include "../common/intermediate-representation-tree/ir_variable_decl_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_compound_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_if_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_for_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_while_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_dowhile_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_assign_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_return_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_switch_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_function_call_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_function_call_expr.hpp"

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
         * @param function - const pointer to the irt function
         * @returns total number of bytes required for the stack of the function for local variables
        */
        size_t computeStackMemory(const IRFunction* function);

        /** 
         * @brief calculates the number of temporary variables
         * @param temporaryExpr - const pointer to the irt temporary
         * @returns number of temporary variables
        */
        size_t computeStackMemory(const IRTemporaryExpr* temporaryExpr);

        /** 
         * @brief calculates the number of local variables in a statement
         * @param stmt - const pointer to the irt statement
         * @returns number of local variables in a statement
        */
        size_t computeStackMemory(const IRStmt* stmt);

        /** 
         * @brief calculates the number of local variables in a variable declaration
         * @param variableDecl - const pointer to the irt variable
         * @returns number of variables in a variable declaration
         * @note initial variable + number of temporary variables
        */
        size_t computeStackMemory(const IRVariableDeclStmt* variableDecl);

        /** 
         * @brief calculates the number of local variables in a compound statement
         * @param compoundStmt - const pointer to irt compound statement
         * @returns number of local variables in a compound statement
        */
        size_t computeStackMemory(const IRCompoundStmt* compoundStmt);

        /** 
         * @brief calculates the number of local variables in an if-statement
         * @param ifStmt - const pointer to the irt if-statement
         * @returns number of local variables in an if-statement
        */
        size_t computeStackMemory(const IRIfStmt* ifStmt);

        /** 
         * @brief calculates the number of local variables in a for-statement
         * @param forStmt - const pointer to the irt for-statement
         * @returns number of local variables in a for-statement
        */
        size_t computeStackMemory(const IRForStmt* forStmt);

        /** 
         * @brief calculates the number of local variables in a while-statement
         * @param whileStmt - const pointer to the irt while-statement
         * @returns number of local variables in a while-statement
        */
        size_t computeStackMemory(const IRWhileStmt* whileStmt);

        /** 
         * @brief calculates the number of local variables in a do-while statement
         * @param dowhileStmt - const pointer to the irt do-while statement
         * @returns number of local variables in a do-while statement
        */
        size_t computeStackMemory(const IRDoWhileStmt* dowhileStmt);

        /** 
         * @brief calculates the number of temporary variables in an assignment statement
         * @param assignStmt - const pointer to the irt assignment statement
         * @returns number of temporary variables in an assignment statement
        */
        size_t computeStackMemory(const IRAssignStmt* assignStmt);

        /** 
         * @brief calculates the number of temporary variables in a return statement
         * @param returnStmt - const pointer to the irt return statement
         * @returns number of temporary variables in a return statement
        */
        size_t computeStackMemory(const IRReturnStmt* returnStmt);

        /** 
         * @brief calculates the number of local variables in a switch-statement
         * @param switchStmt - const pointer to the irt switch-statement
         * @returns number of local variables in a switch-statement
        */
        size_t computeStackMemory(const IRSwitchStmt* switchStmt);

        /** 
         * @brief calculates the number of local variables in a switch-block
         * @param switchBlockStmt - const pointer to the irt switch-block
         * @returns number of local variables in a switch-block
        */
        size_t computeStackMemory(const IRSwitchBlockStmt* switchBlockStmt);

        /** 
         * @brief calculates the number of temporary variables in a function call statement
         * @param callStmt - const pointer to the irt function call statement
         * @returns number of temporary variables in a function call statement
        */
        size_t computeStackMemory(const IRFunctionCallStmt* callStmt);

        /** 
         * @brief calculates the number of temporary variables in a function call expression
         * @param callExpr - const pointer to the irt function call expression
         * @returns number of temporary variables in a function call expression
        */
        size_t computeStackMemory(const IRFunctionCallExpr* callExpr);

        /** 
         * @brief overload for the nodes that don't need any space on the stack
         * @param node - const pointer to irt node (unused)
         * @returns zero
        */
        size_t computeStackMemory([[maybe_unused]] const IRNode* node);
    };
};

#endif