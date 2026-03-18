#include "../ast_while_stmt.hpp"

#include "../defs/ast_defs.hpp"

ASTWhileStmt::ASTWhileStmt(const Token& token) : ASTStmt(token, ASTNodeType::WHILE_STMT) {}

void ASTWhileStmt::setWhileStmt(std::unique_ptr<ASTExpr> cond, std::unique_ptr<ASTStmt> statement){
    conditionExpr = std::move(cond);
    stmt = std::move(statement);
}

void ASTWhileStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}