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

class ReturnChecker final : public ASTVisitor {
public:
    void visit([[maybe_unused]] ASTProgram* program) override;

    void visit([[maybe_unused]] ASTIncludeDir* includeDir) override;

    void visit(ASTFunction* function) override;

    void visit([[maybe_unused]] ASTParameter* parameter) override;

    void visit([[maybe_unused]] ASTVariableDeclStmt* variableDecl) override;

    void visit([[maybe_unused]] ASTAssignStmt* assignStmt) override;

    void visit(ASTCompoundStmt* compoundStmt) override;

    void visit([[maybe_unused]] ASTForStmt* forStmt) override;

    void visit([[maybe_unused]] ASTFunctionCallStmt* callStmt) override;

    void visit(ASTIfStmt* ifStmt) override;

    void visit([[maybe_unused]] ASTReturnStmt* returnStmt) override;

    void visit([[maybe_unused]] ASTWhileStmt* whileStmt) override;

    void visit(ASTDoWhileStmt* dowhileStmt) override;
    
    void visit(ASTSwitchStmt* switchStmt) override;

    void visit(ASTCaseStmt* caseStmt) override;

    void visit(ASTDefaultStmt* defaultStmt) override;
    
    void visit(ASTSwitchBlockStmt* switchBlockStmt) override;

    void visit([[maybe_unused]] ASTBinaryExpr* binaryExpr) override;

    void visit([[maybe_unused]] ASTFunctionCallExpr* callExpr) override;

    void visit([[maybe_unused]] ASTIdExpr* idExpr) override;

    void visit([[maybe_unused]] ASTLiteralExpr* literalExpr) override;

private:
    bool alwaysReturns{ false };
};

#endif