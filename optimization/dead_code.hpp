#ifndef OPTIMIZATION_DEAD_CODE_HPP
#define OPTIMIZATION_DEAD_CODE_HPP

#include "../common/intermediate-representation-tree/ir_function.hpp"
#include "../common/intermediate-representation-tree/ir_compound_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_return_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_if_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_dowhile_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_switch_stmt.hpp"

namespace Optimization {

    /** 
     * @namespace DeadCode
     * @brief Module for the removal of the unused nodes in an irt
    */
    namespace DeadCode {

        /** 
         * @brief reduces the number of statements in a function by eliminating the ones that appear after the node which always returns
         * @param function - pointer to the irt function
        */
        void eliminateDeadCode(IRFunction* function);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         * @param stmt - pointer to the irt statement
         * @returns true if statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRStmt* stmt);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         *
         * reduces the number of statements in a compound statements by eliminating the ones that appear after the node which always returns
         * @param compoundStmt - pointer to the irt compound statement
         * @returns true if compound statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRCompoundStmt* compoundStmt);

        /** 
         * @brief tells the node that represents outter scope to discard statements that appear after it
         * @param returnStmt - pointer to the irt return statement (unused)
         * @returns true
        */
        bool eliminateDeadCode([[maybe_unused]] IRReturnStmt* returnStmt);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         * @param ifStmt - pointer to the irt if-statement
         * @returns true if if-statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRIfStmt* ifStmt);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         * @param dowhileStmt - pointer to the irt do-while statement
         * @returns true if do-while statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRDoWhileStmt* dowhileStmt);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         * @param switchStmt - pointer to the irt switch-statement
         * @returns true if switch-statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRSwitchStmt* switchStmt);

        /** 
         * @brief tells the switch node whether it always returns or not
         * @param caseStmt - pointer to the irt case
         * @returns true if case always returns, false otherwise
        */
        bool eliminateDeadCode(IRCaseStmt* caseStmt);

        /** 
         * @brief tells the switch node whether it always returns or not
         * @param defaultStmt - pointer to the irt default case
         * @returns true if default case always returns, false otherwise
        */
        bool eliminateDeadCode(IRDefaultStmt* defaultStmt);

        /** 
         * @brief tells the case node whether or not it always returns
         *
         * discards statements that appear after the node that always returns
         * @param switchBlockStmt - pointer to the irt switch-block
         * @returns true if switch-block always returns, false otherwise
        */
        bool eliminateDeadCode(IRSwitchBlockStmt* switchBlockStmt); 

        /** 
         * @brief overload for the nodes that can't have return statements
         * @param node - pointer to the irt node (unused)
         * @returns false
        */
        bool eliminateDeadCode([[maybe_unused]] IRNode* node);
    };
};

#endif