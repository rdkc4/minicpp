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

/**
 * @class StatementIntermediateRepresentation
 * @brief turns ast statement into irt statement
*/
class StatementIntermediateRepresentation {
public:
    /** 
     * @brief Creates the instance of the statement intermediate representation
    */
    StatementIntermediateRepresentation();

    /**
     * @brief turns ast statement into irt statement
     * @param _statement - const pointer to the ast statement
     * @returns pointer to the irt statement
    */
    std::unique_ptr<IRStmt> statement(const ASTStmt* _statement);
    
    /**
     * @brief turns ast variable declaration into irt variable declaration
     * @param _variable - const pointer to the ast variable declaration
     * @returns irt pointer to the variable declaratiom
    */
    std::unique_ptr<IRVariableDeclStmt> variable(const ASTVariableDeclStmt* _variable);

    /**
     * @brief turns ast if-statement into irt if-statement
     * @param _if - const pointer to the ast if-statement
     * @returns pointer to the irt if-statement
    */
    std::unique_ptr<IRIfStmt> ifStatement(const ASTIfStmt* _if);

    /**
     * @brief turns ast compound statement into irt compound statement
     * @param _compound - const pointer to the ast compound statement
     * @returns pointer to the irt compound statement
    */
    std::unique_ptr<IRCompoundStmt> compoundStatement(const ASTCompoundStmt* _compound);

    /**
     * @brief turns ast assignment statement into irt assignment statement
     * @param _assignment - const pointer to the ast assignment statement
     * @returns pointer to the irt statement
    */
    std::unique_ptr<IRAssignStmt> assignmentStatement(const ASTAssignStmt* _assignment);

    /**
     * @brief turns ast return statement into irt return statement
     * @param _return - const pointer to the ast return statement
     * @returns pointer to the irt return statement
    */
    std::unique_ptr<IRReturnStmt> returnStatement(const ASTReturnStmt* _return);

    /**
     * @brief turns ast function-call statement into irt function-call statement
     * @param _call - const pointer to the ast function-call statement
     * @returns pointer to the irt function-call statement
    */
    std::unique_ptr<IRFunctionCallStmt> functionCallStatement(const ASTFunctionCallStmt* _call);

    /**
     * @brief turns ast while-statement into irt while-statement
     * @param _while - const pointer to the ast while-statement
     * @returns pointer to the irt while-statement 
    */
    std::unique_ptr<IRWhileStmt> whileStatement(const ASTWhileStmt* _while);

    /**
     * @brief turns ast for-statement into irt for-statement
     * @param _for - const pointer to the ast for-statement
     * @returns pointer to the irt for-statement
    */
    std::unique_ptr<IRForStmt> forStatement(const ASTForStmt* _for);

    /**
     * @brief turns ast do-while statement into irt do-while statement
     * @param _dowhile - const pointer to the ast do-while statement
     * @returns pointer to the irt do-while statement
    */
    std::unique_ptr<IRDoWhileStmt> doWhileStatement(const ASTDoWhileStmt* _dowhile);

    /**
     * @brief turns ast switch-statement into irt switch-statement
     * @param _switch - const pointer to the ast switch-statement
     * @returns pointer to the irt switch-statement
    */
    std::unique_ptr<IRSwitchStmt> switchStatement(const ASTSwitchStmt* _switch);

    /**
     * @brief turns ast case into irt case
     * @param _astCase - const pointer to the ast case
     * @returns pointer to the irt case
    */
    std::unique_ptr<IRCaseStmt> _case(const ASTCaseStmt* _astCase);

    /**
     * @brief turns ast default case into irt default case
     * @param _astDefault - const pointer to the ast default case
     * @returns pointer to the irt default case
    */
    std::unique_ptr<IRDefaultStmt> _default(const ASTDefaultStmt* _astDefault);

    /**
     * @brief turns ast switch-block into irt switch-block
     * @param _block - const pointer to the ast switch-block
     * @returns pointer to the irt switch-block
    */
    std::unique_ptr<IRSwitchBlockStmt> switchBlock(const ASTSwitchBlockStmt* _block);

private:
    /// intermediate representation specialized for expressions
    ExpressionIntermediateRepresentation expIR;

};

#endif
