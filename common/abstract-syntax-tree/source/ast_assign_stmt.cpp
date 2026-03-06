#include "../ast_assign_stmt.hpp"

ASTAssignStmt::ASTAssignStmt(const Token& token, ASTNodeType ntype) : ASTStmt(token, ntype) {}

void ASTAssignStmt::setVariable(std::unique_ptr<ASTIdExpr> var){
    variable = std::move(var);
}

void ASTAssignStmt::setExp(std::unique_ptr<ASTExpr> nexp){
    exp = std::move(nexp);
}

void ASTAssignStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}