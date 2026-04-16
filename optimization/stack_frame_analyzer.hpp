#ifndef STACK_FRAME_ANALYZER_HPP
#define STACK_FRAME_ANALYZER_HPP

#include "../common/visitor/ir_visitor.hpp"
#include "../common/intermediate-representation-tree/ir_program.hpp"
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
#include "../thread-pool/thread_pool.hpp"

/**
 * @namespace Optimization::sfa
 * @brief module for calculating the size of the stack
*/
namespace Optimization::sfa {
    /**
    * @class StackFrameAnalyzer
    * @brief calculates the size of the stack for each function
    */
    class StackFrameAnalyzer final : public IR::visitor::IRVisitor {
    public:
        /**
         * @brief creates the instance of the stack frame analyzer
         * @param threadPool - reference to a thread pool for parallel function stack analysis
        */
        StackFrameAnalyzer(ThreadPool& threadPool);

        /**
         * @brief starts the stack analysis for all functions
         * @param program - pointer to the program
        */
        void visit(IR::node::IRProgram* program) override;

        /**
         * @brief calculates the size of the stack required for the function
         * @param function - pointer to the function
        */
        void visit(IR::node::IRFunction* function) override;

        /**
         * @brief intentionally empty, not used for stack size evaluation
         * @param parameter - pointer to the parameter
        */
        void visit([[maybe_unused]] IR::node::IRParameter* parameter) override { /*empty*/ };

        /**
         * @brief calculates the size of the stack required for the variable declaration
         * @param variableDecl - pointer to the variable declaration
        */
        void visit(IR::node::IRVariableDeclStmt* variableDecl) override;

        /**
         * @brief calculates the size of the stack required for the assign statement
         * @param assignStmt - pointer to the assign statement
        */
        void visit(IR::node::IRAssignStmt* assignStmt) override;

        /**
         * @brief calculates the size of the stack required for the compound statement
         * @param compoundStmt - pointer to the compound statement
        */
        void visit(IR::node::IRCompoundStmt* compoundStmt) override;

        /**
         * @brief calculates the size of the stack required for the for statement
         * @param forStmt - pointer to the for statement
        */
        void visit(IR::node::IRForStmt* forStmt) override;

        /**
         * @brief calculates the size of the stack required for the function call statement
         * @param callStmt - pointer to the function call statement
        */
        void visit(IR::node::IRFunctionCallStmt* callStmt) override;

        /**
         * @brief calculates the size of the stack required for the if statement
         * @param ifStmt - pointer to the if statement
        */
        void visit(IR::node::IRIfStmt* ifStmt) override;

        /**
         * @brief calculates the size of the stack required for the return statement
         * @param returnStmt - pointer to the return statement
        */
        void visit(IR::node::IRReturnStmt* returnStmt) override;

        /**
         * @brief calculates the size of the stack required for the while statement
         * @param whileStmt - pointer to the while statement
        */
        void visit(IR::node::IRWhileStmt* whileStmt) override;

        /**
         * @brief calculates the size of the stack required for the do-while statement
         * @param dowhileStmt - pointer to the do-while statement
        */
        void visit(IR::node::IRDoWhileStmt* dowhileStmt) override;

        /**
         * @brief calculates the size of the stack required for the switch statement
         * @param switchStmt - pointer to the switch statement
        */
        void visit(IR::node::IRSwitchStmt* switchStmt) override;

        /**
         * @brief calculates the size of the stack required for the case statement
         * @param caseStmt - pointer to the case statement
        */
        void visit(IR::node::IRCaseStmt* caseStmt) override;

        /**
         * @brief calculates the size of the stack required for the default statement
         * @param defaultStmt - pointer to the default statement
        */
        void visit(IR::node::IRDefaultStmt* defaultStmt) override;

        /**
         * @brief calculates the size of the stack required for the switch-block statement
         * @param switchBlockStmt - pointer to the switch-block statement
        */
        void visit(IR::node::IRSwitchBlockStmt* switchBlockStmt) override;

        /**
         * @brief intentionally empty, not used for stack size evaluation
         * @param binaryExpr - pointer to the binary expression
        */
        void visit([[maybe_unused]] IR::node::IRBinaryExpr* binaryExpr) override { /*empty*/ };

        /**
         * @brief calculates the size of the stack required for the function call expression
         * @param callExpr - pointer to the function call expression
        */
        void visit(IR::node::IRFunctionCallExpr* callExpr) override;

        /**
         * @brief intentionally empty, not used for stack size evaluation
         * @param idExpr - pointer to the id expression
        */
        void visit([[maybe_unused]] IR::node::IRIdExpr* idExpr) override { /*empty*/ };

        /**
         * @brief intentionally empty, not used for stack size evaluation
         * @param literalExpr - pointer to the literal expression
        */
        void visit([[maybe_unused]] IR::node::IRLiteralExpr* literalExpr) override { /*empty*/ };

        /**
         * @brief calculates the size of the stack required for the temporary expression
         * @param tempExpr - pointer to the temporary expression
        */
        void visit(IR::node::IRTemporaryExpr* tempExpr) override;

    private:
        /// reference to a thread pool for parallel function stack analysis
        ThreadPool& threadPool;

        /// number of required variables
        static thread_local size_t variableCounter;

        /// size of the register
        constexpr static size_t regSize{8};

    };

}

#endif