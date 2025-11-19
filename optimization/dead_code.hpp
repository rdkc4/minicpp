#ifndef OPTIMIZATION_DEAD_CODE_HPP
#define OPTIMIZATION_DEAD_CODE_HPP

#include "../common/intermediate-representation-tree/IRFunction.hpp"
#include "../common/intermediate-representation-tree/IRCompoundSt.hpp"
#include "../common/intermediate-representation-tree/IRReturnSt.hpp"
#include "../common/intermediate-representation-tree/IRIfSt.hpp"
#include "../common/intermediate-representation-tree/IRDoWhileSt.hpp"
#include "../common/intermediate-representation-tree/IRSwitchSt.hpp"

namespace Optimization {

    /** 
     * @namespace DeadCode
     * @brief Module for the removal of the unused nodes in an irt
    */
    namespace DeadCode {

        /** 
         * @brief reduces the number of statements in a function by eliminating the ones that appear after the node which always returns
         * @param _function - pointer to the irt function
         * @returns void
        */
        void eliminateDeadCode(IRFunction* _function);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         * @param _stmt - pointer to the irt statement
         * @returns true if statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRStatement* _stmt);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         *
         * reduces the number of statements in a compound statements by eliminating the ones that appear after the node which always returns
         * @param _compound - pointer to the irt compound statement
         * @returns true if compound statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRCompoundSt* _compound);

        /** 
         * @brief tells the node that represents outter scope to discard statements that appear after it
         * @param _return - pointer to the irt return statement (unused)
         * @returns true
        */
        bool eliminateDeadCode([[maybe_unused]] IRReturnSt* _return);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         * @param _if - pointer to the irt if-statement
         * @returns true if if-statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRIfSt* _if);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         * @param _dowhile - pointer to the irt do-while statement
         * @returns true if do-while statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRDoWhileSt* _dowhile);

        /** 
         * @brief tells the node that represents outter scope whether or not to discard statements that appear after it
         * @param _switch - pointer to the irt switch-statement
         * @returns true if switch-statement always returns, false otherwise
        */
        bool eliminateDeadCode(IRSwitchSt* _switch);

        /** 
         * @brief tells the switch node whether it always returns or not
         * @param _case - pointer to the irt case
         * @returns true if case always returns, false otherwise
        */
        bool eliminateDeadCode(IRCaseSt* _case);

        /** 
         * @brief tells the switch node whether it always returns or not
         * @param _default - pointer to the irt default case
         * @returns true if default case always returns, false otherwise
        */
        bool eliminateDeadCode(IRDefaultSt* _default);

        /** 
         * @brief tells the case node whether or not it always returns
         *
         * discards statements that appear after the node that always returns
         * @param _swBlock - pointer to the irt switch-block
         * @returns true if switch-block always returns, false otherwise
        */
        bool eliminateDeadCode(IRSwitchBlock* _swBlock); 

        /** 
         * @brief overload for the nodes that can't have return statements
         * @param _node - pointer to the irt node (unused)
         * @returns false
        */
        bool eliminateDeadCode([[maybe_unused]] IRNode* _node);
    };
};

#endif