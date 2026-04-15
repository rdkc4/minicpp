#ifndef RETURN_CHECKER_HPP
#define RETURN_CHECKER_HPP

#include "../common/visitor/ast_visitor.hpp"
#include "../common/abstract-syntax-tree/ast_program.hpp"
#include "../common/abstract-syntax-tree/ast_include_dir.hpp"
#include "../common/abstract-syntax-tree/ast_function.hpp"
#include "../common/abstract-syntax-tree/ast_parameter.hpp"
#include "../common/abstract-syntax-tree/ast_variable_decl_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_assign_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_compound_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_for_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_function_call_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_if_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_return_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_while_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_dowhile_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_switch_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_case_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_default_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_switch_block_stmt.hpp"
#include "../common/abstract-syntax-tree/ast_binary_expr.hpp"
#include "../common/abstract-syntax-tree/ast_function_call_expr.hpp"
#include "../common/abstract-syntax-tree/ast_id_expr.hpp"
#include "../common/abstract-syntax-tree/ast_literal_expr.hpp"

/**
 * @class ReturnChecker
 * @brief checks if the node returns always or not
*/
class ReturnChecker final : public AST::visitor::ASTVisitor {
public:
    /**
     * @brief intentionally empty, cannot return
     * @param program - pointer to the program
    */
    void visit([[maybe_unused]] AST::node::ASTProgram* program) override;

    /**
     * @brief intentionally empty, cannot return
     * @param includeDir - pointer to the include directive
    */
    void visit([[maybe_unused]] AST::node::ASTIncludeDir* includeDir) override;

    /**
     * @brief checks if the function always returns
     * @param function - pointer to the function
    */
    void visit(AST::node::ASTFunction* function) override;

    /**
     * @brief intentionally empty, cannot return
     * @param parameter - pointer to the parameter
    */
    void visit([[maybe_unused]] AST::node::ASTParameter* parameter) override;

    /**
     * @brief intentionally empty, cannot return
     * @param variableDecl - pointer to the variable declaration
    */
    void visit([[maybe_unused]] AST::node::ASTVariableDeclStmt* variableDecl) override;

    /**
     * @brief intentionally empty, cannot return
     * @param assignStmt - pointer to the assignment statement
    */
    void visit([[maybe_unused]] AST::node::ASTAssignStmt* assignStmt) override;

    /**
     * @brief checks if the compound statement always returns
     * @param compoundStmt - pointer to the compound statement
    */
    void visit(AST::node::ASTCompoundStmt* compoundStmt) override;

    /**
     * @brief intentionally empty, cannot evaluate the condition at compile-time at the moment
     * @param forStmt - pointer to the for statement
    */
    void visit([[maybe_unused]] AST::node::ASTForStmt* forStmt) override;

    /**
     * @brief intentionally empty, cannot return
     * @param callStmt - pointer to the function call statement
    */
    void visit([[maybe_unused]] AST::node::ASTFunctionCallStmt* callStmt) override;

    /**
     * @brief checks if the if-statement always returns
     * @param ifStmt - pointer to the if statement
    */
    void visit(AST::node::ASTIfStmt* ifStmt) override;

    /**
     * @brief always returns
     * @param returnStmt - pointer to the return statement
    */
    void visit([[maybe_unused]] AST::node::ASTReturnStmt* returnStmt) override;

    /**
     * @brief intentionally empty, cannot evaluate the condition at compile-time at the moment
     * @param whileStmt - pointer to the while statement
    */
    void visit([[maybe_unused]] AST::node::ASTWhileStmt* whileStmt) override;

    /**
     * @brief checks if the do-while statement always returns
     * @param dowhileStmt - pointer to the do-while statement
    */
    void visit(AST::node::ASTDoWhileStmt* dowhileStmt) override;
    
    /**
     * @brief checks if the switch statement always returns
     * @param switchStmt - pointer to the switch statement
    */
    void visit(AST::node::ASTSwitchStmt* switchStmt) override;

    /**
     * @brief checks if the case statement always returns
     * @param caseStmt - pointer to the case statement
    */
    void visit(AST::node::ASTCaseStmt* caseStmt) override;

    /**
     * @brief checks if the default statement always returns
     * @param defaultStmt - pointer to the default statement
    */
    void visit(AST::node::ASTDefaultStmt* defaultStmt) override;
    
    /**
     * @brief checks if the switch-block statement always returns
     * @param switchBlockStmt - pointer to the switch-block statement
    */
    void visit(AST::node::ASTSwitchBlockStmt* switchBlockStmt) override;

    /**
     * @brief intentionally empty, cannot return
     * @param binaryExpr - pointer to the binary expression
    */
    void visit([[maybe_unused]] AST::node::ASTBinaryExpr* binaryExpr) override;

    /**
     * @brief intentionally empty, cannot return
     * @param callExpr - pointer to the function call expression
    */
    void visit([[maybe_unused]] AST::node::ASTFunctionCallExpr* callExpr) override;

    /**
     * @brief intentionally empty, cannot return
     * @param idExpr - pointer to the id expression
    */
    void visit([[maybe_unused]] AST::node::ASTIdExpr* idExpr) override;

    /**
     * @brief intentionally empty, cannot return
     * @param literalExpr - pointer to the literal expression
    */
    void visit([[maybe_unused]] AST::node::ASTLiteralExpr* literalExpr) override;

private:
    /// flag if node returns or not
    bool alwaysReturns{ false };
};

#endif