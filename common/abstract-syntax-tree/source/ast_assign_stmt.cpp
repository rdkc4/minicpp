#include "../ast_assign_stmt.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTAssignStmt::ASTAssignStmt(const syntax::Token& token) 
    : ASTStmt(token, syntax::ast::ASTNodeType::ASSIGN_STMT) {}

void syntax::ast::ASTAssignStmt::setVariableIdExpr(std::unique_ptr<syntax::ast::ASTIdExpr> idExpr){
    variableIdExpr = std::move(idExpr);
}

void syntax::ast::ASTAssignStmt::setAssignedExpr(std::unique_ptr<syntax::ast::ASTExpr> expr){
    assignedExpr = std::move(expr);
}

void syntax::ast::ASTAssignStmt::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}