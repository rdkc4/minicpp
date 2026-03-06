#include "../ast_dowhile_stmt.hpp"

ASTDoWhileStmt::ASTDoWhileStmt(const Token& token, ASTNodeType ntype) : ASTStmt(token, ntype) {}

const ASTStmt* ASTDoWhileStmt::getStatement() const noexcept {
    return statement.get();
}

void ASTDoWhileStmt::setDoWhile(std::unique_ptr<ASTExpr> cond, std::unique_ptr<ASTStmt> st){
    condition = std::move(cond);
    statement = std::move(st);
}

void ASTDoWhileStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}