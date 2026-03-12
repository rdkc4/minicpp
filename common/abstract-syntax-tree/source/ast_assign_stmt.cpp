#include "../ast_assign_stmt.hpp"

#include "../defs/ast_defs.hpp"

ASTAssignStmt::ASTAssignStmt(const Token& token) : ASTStmt(token, ASTNodeType::ASSIGNMENT_STATEMENT) {}

void ASTAssignStmt::setVariableIdExpr(std::unique_ptr<ASTIdExpr> idExpr){
    variableIdExpr = std::move(idExpr);
}

void ASTAssignStmt::setAssignedExpr(std::unique_ptr<ASTExpr> expr){
    assignedExpr = std::move(expr);
}

void ASTAssignStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}