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
         * @param _function - pointer to the irt function
        */
        void eliminateDeadCode(IRFunction* _function);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         * @param _stmt - pointer to the irt statement
         * @returns true if statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRStmt* _stmt);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         *
         * reduces the number of statements in a compound statements by eliminating the ones that appear after the node which always returns
         * @param _compound - pointer to the irt compound statement
         * @returns true if compound statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRCompoundStmt* _compound);

        /** 
         * @brief tells the node that represents outter scope to discard statements that appear after it
         * @param _return - pointer to the irt return statement (unused)
         * @returns true
        */
        bool eliminateDeadCode([[maybe_unused]] IRReturnStmt* _return);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         * @param _if - pointer to the irt if-statement
         * @returns true if if-statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRIfStmt* _if);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         * @param _dowhile - pointer to the irt do-while statement
         * @returns true if do-while statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRDoWhileStmt* _dowhile);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         * @param _switch - pointer to the irt switch-statement
         * @returns true if switch-statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRSwitchStmt* _switch);

        /** 
         * @brief tells the switch node whether it always returns or not
         * @param _case - pointer to the irt case
         * @returns true if case always returns, false otherwise
        */
        bool eliminateDeadCode(IRCaseStmt* _case);

        /** 
         * @brief tells the switch node whether it always returns or not
         * @param _default - pointer to the irt default case
         * @returns true if default case always returns, false otherwise
        */
        bool eliminateDeadCode(IRDefaultStmt* _default);

        /** 
         * @brief tells the case node whether or not it always returns
         *
         * discards statements that appear after the node that always returns
         * @param _swBlock - pointer to the irt switch-block
         * @returns true if switch-block always returns, false otherwise
        */
        bool eliminateDeadCode(IRSwitchBlockStmt* _swBlock); 

        /** 
         * @brief overload for the nodes that can't have return statements
         * @param _node - pointer to the irt node (unused)
         * @returns false
        */
        bool eliminateDeadCode([[maybe_unused]] IRNode* _node);
    };
};

#endif