#ifndef STATEMENT_INTERMEDIATE_REPRESENTATION_HPP
#define STATEMENT_INTERMEDIATE_REPRESENTATION_HPP

#include <memory>

#include "../common/abstract-syntax-tree/ast_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_variable_decl_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_if_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_compound_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_assign_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_return_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_while_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_for_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_dowhile_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_switch_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_function_call_stmt.hpp"

#include "../common/intermediate-representation-tree/ir_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_variable_decl_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_if_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_compound_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_assign_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_return_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_while_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_for_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_dowhile_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_switch_stmt.hpp"
#include "../common/intermediate-representation-tree/ir_function_call_stmt.hpp"
#include "expression_intermediate_representation.hpp"

namespace IR {
    /**
     * @class StatementIntermediateRepresentation
     * @brief turns ast statement into irt statement
    */
    class StatementIntermediateRepresentation {
    public:
        /** 
         * @brief Creates the instance of the statement intermediate representation
        */
        StatementIntermediateRepresentation() = default;

        /**
         * @brief turns ast statement into irt statement
         * @param astStmt - const pointer to the ast statement
         * @returns pointer to the irt statement
        */
        std::unique_ptr<node::IRStmt> transformStmt(const AST::node::ASTStmt* astStmt);
        
        /**
         * @brief turns ast variable declaration into irt variable declaration
         * @param astVariableDecl - const pointer to the ast variable declaration
         * @returns irt pointer to the variable declaratiom
        */
        std::unique_ptr<node::IRVariableDeclStmt> 
        transformVariableDeclStmt(const AST::node::ASTVariableDeclStmt* astVariableDecl);

        /**
         * @brief turns ast if-statement into irt if-statement
         * @param astIfStmt - const pointer to the ast if-statement
         * @returns pointer to the irt if-statement
        */
        std::unique_ptr<node::IRIfStmt> 
        transformIfStmt(const AST::node::ASTIfStmt* astIfStmt);

        /**
         * @brief turns ast compound statement into irt compound statement
         * @param astCompoundStmt - const pointer to the ast compound statement
         * @returns pointer to the irt compound statement
        */
        std::unique_ptr<node::IRCompoundStmt> 
        transformCompoundStmt(const AST::node::ASTCompoundStmt* astCompoundStmt);

        /**
         * @brief turns ast assignment statement into irt assignment statement
         * @param astAssignStmt - const pointer to the ast assignment statement
         * @returns pointer to the irt statement
        */
        std::unique_ptr<node::IRAssignStmt> 
        transformAssignStmt(const AST::node::ASTAssignStmt* astAssignStmt);

        /**
         * @brief turns ast return statement into irt return statement
         * @param astReturnStmt - const pointer to the ast return statement
         * @returns pointer to the irt return statement
        */
        std::unique_ptr<node::IRReturnStmt> 
        transformReturnStmt(const AST::node::ASTReturnStmt* astReturnStmt);

        /**
         * @brief turns ast function-call statement into irt function-call statement
         * @param astCallStmt - const pointer to the ast function-call statement
         * @returns pointer to the irt function-call statement
        */
        std::unique_ptr<node::IRFunctionCallStmt> 
        transformFunctionCallStmt(const AST::node::ASTFunctionCallStmt* astCallStmt);

        /**
         * @brief turns ast while-statement into irt while-statement
         * @param astWhileStmt - const pointer to the ast while-statement
         * @returns pointer to the irt while-statement 
        */
        std::unique_ptr<node::IRWhileStmt> 
        transformWhileStmt(const AST::node::ASTWhileStmt* astWhileStmt);

        /**
         * @brief turns ast for-statement into irt for-statement
         * @param astForStmt - const pointer to the ast for-statement
         * @returns pointer to the irt for-statement
        */
        std::unique_ptr<node::IRForStmt> 
        transformForStmt(const AST::node::ASTForStmt* astForStmt);

        /**
         * @brief turns ast do-while statement into irt do-while statement
         * @param astDowhileStmt - const pointer to the ast do-while statement
         * @returns pointer to the irt do-while statement
        */
        std::unique_ptr<node::IRDoWhileStmt> 
        transformDoWhileStmt(const AST::node::ASTDoWhileStmt* astDowhileStmt);

        /**
         * @brief turns ast switch-statement into irt switch-statement
         * @param astSwitchStmt - const pointer to the ast switch-statement
         * @returns pointer to the irt switch-statement
        */
        std::unique_ptr<node::IRSwitchStmt> 
        transformSwitchStmt(const AST::node::ASTSwitchStmt* astSwitchStmt);

        /**
         * @brief turns ast case into irt case
         * @param astCaseStmt - const pointer to the ast case
         * @returns pointer to the irt case
        */
        std::unique_ptr<node::IRCaseStmt> 
        transformCaseStmt(const AST::node::ASTCaseStmt* astCaseStmt);

        /**
         * @brief turns ast default case into irt default case
         * @param astDefaultStmt - const pointer to the ast default case
         * @returns pointer to the irt default case
        */
        std::unique_ptr<node::IRDefaultStmt> 
        transformDefaultStmt(const AST::node::ASTDefaultStmt* astDefaultStmt);

        /**
         * @brief turns ast switch-block into irt switch-block
         * @param astSwitchBlockStmt - const pointer to the ast switch-block
         * @returns pointer to the irt switch-block
        */
        std::unique_ptr<node::IRSwitchBlockStmt> 
        transformSwitchBlockStmt(const AST::node::ASTSwitchBlockStmt* astSwitchBlockStmt);

    private:
        /// intermediate representation specialized for expressions
        ExpressionIntermediateRepresentation exprIR;

    };
}

#endif