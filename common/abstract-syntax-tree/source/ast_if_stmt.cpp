#include "../ast_if_stmt.hpp"

ASTIfStmt::ASTIfStmt(const Token& token, ASTNodeType ntype) : ASTStmt(token, ntype) {}

const std::vector<std::unique_ptr<ASTExpr>>& ASTIfStmt::getConditions() const noexcept {
    return conditions;
}
const std::vector<std::unique_ptr<ASTStmt>>& ASTIfStmt::getStatements() const noexcept {
    return statements;
}

void ASTIfStmt::addIf(std::unique_ptr<ASTExpr> condition, std::unique_ptr<ASTStmt> statement){
    conditions.push_back(std::move(condition));
    statements.push_back(std::move(statement));
}

void ASTIfStmt::addElse(std::unique_ptr<ASTStmt> statement){
    statements.push_back(std::move(statement));
}

bool ASTIfStmt::hasElse() const noexcept {
    return statements.size() > conditions.size();
}

void ASTIfStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}