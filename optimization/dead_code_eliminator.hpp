#ifndef DEAD_CODE_ELIMINATOR_HPP
#define DEAD_CODE_ELIMINATOR_HPP

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
 * @namespace optimization::dce
 * @brief module for the elimination of unreachable code
*/
namespace optimization::dce {
    /**
     * @class DeadCodeEliminator
     * @brief eliminates unreachable code
    */
    class DeadCodeEliminator final : public ir::IRVisitor {
    public:
        /**
         * @brief creates the instance of the dead code eliminator
         * @param threadPool - reference to a thread pool for parallel dead code elimination
        */
        DeadCodeEliminator(util::concurrency::ThreadPool& threadPool);

        /**
         * @brief starts the dead code elimination for all functions
         * @param program - pointer to the program
        */
        void visit(ir::IRProgram* program) override;

        /**
         * @brief eliminates the dead code of the function
         * @param function - pointer to the function
        */
        void visit(ir::IRFunction* function) override;

        /**
         * @brief intentionally empty, cannot return
         * @param parameter - pointer to the parameter
        */
        void visit([[maybe_unused]] ir::IRParameter* parameter) override { /*empty*/ };

        /**
         * @brief intentionally empty, cannot return
         * @param variableDecl - pointer to the variable declaration
        */
        void visit([[maybe_unused]] ir::IRVariableDeclStmt* variableDecl) override { /*empty*/ };

        /**
         * @brief intentionally empty, cannot return
         * @param assignStmt - pointer to the assign statement
        */
        void visit([[maybe_unused]] ir::IRAssignStmt* assignStmt) override { /*empty*/ };

        /**
         * @brief eliminates the dead code of the compound statement
         * @param compoundStmt - pointer to the compound statement
        */
        void visit(ir::IRCompoundStmt* compoundStmt) override;

        /**
         * @brief intentionally empty, cannot return (compile-time condition evaluation is not available yet)
         * @param forStmt - pointer to the for statement
        */
        void visit([[maybe_unused]] ir::IRForStmt* forStmt) override { /*empty*/ };

        /**
         * @brief intentionally empty, cannot return
         * @param callStmt - pointer to the function call statement
        */
        void visit([[maybe_unused]] ir::IRFunctionCallStmt* callStmt) override { /*empty*/ };

        /**
         * @brief eliminates the dead code of the if statement
         * @param ifStmt - pointer to the if statement
        */
        void visit(ir::IRIfStmt* ifStmt) override;

        /**
         * @brief always returns
         * @param returnStmt - pointer to the return statement
        */
        void visit([[maybe_unused]] ir::IRReturnStmt* returnStmt) override;

        /**
         * @brief intentionally empty, cannot return (compile-time condition evaluation is not available yet)
         * @param whileStmt - pointer to the while statement
        */
        void visit([[maybe_unused]] ir::IRWhileStmt* whileStmt) override { /*empty*/ };

        /**
         * @brief eliminates the dead code of the do-while statement
         * @param dowhileStmt - pointer to the do-while statement
        */
        void visit(ir::IRDoWhileStmt* dowhileStmt) override;

        /**
         * @brief eliminates the dead code of the switch statement
         * @param switchStmt - pointer to the switch statement
        */
        void visit(ir::IRSwitchStmt* switchStmt) override;

        /**
         * @brief eliminates the dead code of the case statement
         * @param caseStmt - pointer to the case statement
        */
        void visit(ir::IRCaseStmt* caseStmt) override;

        /**
         * @brief eliminates the dead code of the default statement
         * @param defaultStmt - pointer to the default statement
        */
        void visit(ir::IRDefaultStmt* defaultStmt) override;

        /**
         * @brief eliminates the dead code of the switch-block statement
         * @param switchBlockStmt - pointer to the switch-block statement
        */
        void visit(ir::IRSwitchBlockStmt* switchBlockStmt) override;

        /**
         * @brief intentionally empty, cannot return
         * @param binaryExpr - pointer to the binary expression
        */
        void visit([[maybe_unused]] ir::IRBinaryExpr* binaryExpr) override { /*empty*/ };

        /**
         * @brief intentionally empty, cannot return
         * @param callExpr - pointer to the function call expression
        */
        void visit([[maybe_unused]] ir::IRFunctionCallExpr* callExpr) override { /*empty*/ };

        /**
         * @brief intentionally empty, cannot return
         * @param idExpr - pointer to the id expression
        */
        void visit([[maybe_unused]] ir::IRIdExpr* idExpr) override { /*empty*/ };

        /**
         * @brief intentionally empty, cannot return
         * @param literalExpr - pointer to the literal expression
        */
        void visit([[maybe_unused]] ir::IRLiteralExpr* literalExpr) override { /*empty*/ };

        /**
         * @brief intentionally empty, cannot return
         * @param tempExpr - pointer to the temporary expression
        */
        void visit([[maybe_unused]] ir::IRTemporaryExpr* tempExpr) override { /*empty*/ };

    private:
        /// reference to a thread pool for parallel dead code elimination
        util::concurrency::ThreadPool& threadPool;

        /// flag if current node always returns
        static thread_local bool alwaysReturns;

    };

}

#endif