#ifndef AST_DUMPER_HPP
#define AST_DUMPER_HPP

#include <ostream>
#include <string_view>

#include "../visitor/ast_visitor.hpp"
#include "ast_node.hpp"
#include "ast_program.hpp"
#include "ast_include_dir.hpp"
#include "ast_function.hpp"
#include "ast_parameter.hpp"
#include "ast_variable_decl_stmt.hpp"
#include "ast_assign_stmt.hpp"
#include "ast_compound_stmt.hpp"
#include "ast_for_stmt.hpp"
#include "ast_function_call_stmt.hpp"
#include "ast_if_stmt.hpp"
#include "ast_return_stmt.hpp"
#include "ast_while_stmt.hpp"
#include "ast_dowhile_stmt.hpp"
#include "ast_switch_stmt.hpp"
#include "ast_case_stmt.hpp"
#include "ast_default_stmt.hpp"
#include "ast_switch_block_stmt.hpp"
#include "ast_binary_expr.hpp"
#include "ast_function_call_expr.hpp"
#include "ast_id_expr.hpp"
#include "ast_literal_expr.hpp"

class ASTDumper final : public ASTVisitor {
public:
    ASTDumper(std::ostream& out);

    void visit(ASTProgram* program) override;

    void visit(ASTIncludeDir* lib) override;

    void visit(ASTFunction* function) override;

    void visit(ASTParameter* parameter) override;

    void visit(ASTVariableDeclStmt* variableDecl) override;

    void visit(ASTAssignStmt* assignStmt) override;

    void visit(ASTCompoundStmt* compoundStmt) override;

    void visit(ASTForStmt* forStmt) override;

    void visit(ASTFunctionCallStmt* callStmt) override;

    void visit(ASTIfStmt* ifStmt) override;

    void visit(ASTReturnStmt* returnStmt) override;

    void visit(ASTWhileStmt* whileStmt) override;

    void visit(ASTDoWhileStmt* dowhileStmt) override;
    
    void visit(ASTSwitchStmt* switchStmt) override;

    void visit(ASTCaseStmt* caseStmt) override;

    void visit(ASTDefaultStmt* defaultStmt) override;
    
    void visit(ASTSwitchBlockStmt* switchBlockStmt) override;

    void visit(ASTBinaryExpr* binaryExpr) override;

    void visit(ASTFunctionCallExpr* callExpr) override;

    void visit(ASTIdExpr* idExpr) override;

    void visit(ASTLiteralExpr* literalExpr) override;

private:
    std::ostream& out;

    int indent = 0;

    void dumpIndent();

    void dumpNode(const ASTNode* node);

    void dumpNode(std::string_view nodeLabel);

};

#endif