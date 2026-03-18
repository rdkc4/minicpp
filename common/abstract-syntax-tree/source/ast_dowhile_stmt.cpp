#include "../ast_dowhile_stmt.hpp"

#include "../defs/ast_defs.hpp"

ASTDoWhileStmt::ASTDoWhileStmt(const Token& token) : ASTStmt(token, ASTNodeType::DOWHILE_STMT) {}

void ASTDoWhileStmt::setDoWhile(std::unique_ptr<ASTExpr> condExpr, std::unique_ptr<ASTStmt> statement){
    conditionExpr = std::move(condExpr);
    stmt = std::move(statement);
}

void ASTDoWhileStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}