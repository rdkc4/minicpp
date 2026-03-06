#include "../ast_while_stmt.hpp"

ASTWhileStmt::ASTWhileStmt(const Token& token, ASTNodeType ntype) : ASTStmt(token, ntype) {}

const ASTStmt* ASTWhileStmt::getStatement() const noexcept {
    return statement.get();
}

void ASTWhileStmt::setWhile(std::unique_ptr<ASTExpr> cond, std::unique_ptr<ASTStmt> st){
    condition = std::move(cond);
    statement = std::move(st);
}

void ASTWhileStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}