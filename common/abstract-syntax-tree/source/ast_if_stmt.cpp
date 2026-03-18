#include "../ast_if_stmt.hpp"

#include "../defs/ast_defs.hpp"

ASTIfStmt::ASTIfStmt(const Token& token) : ASTStmt(token, ASTNodeType::IF_STMT) {}

const std::vector<std::unique_ptr<ASTExpr>>& ASTIfStmt::getConditionExprs() const noexcept {
    return conditionExprs;
}
const std::vector<std::unique_ptr<ASTStmt>>& ASTIfStmt::getStmts() const noexcept {
    return stmts;
}

void ASTIfStmt::addIfStmt(std::unique_ptr<ASTExpr> condition, std::unique_ptr<ASTStmt> statement){
    conditionExprs.push_back(std::move(condition));
    stmts.push_back(std::move(statement));
}

void ASTIfStmt::addElseStmt(std::unique_ptr<ASTStmt> statement){
    stmts.push_back(std::move(statement));
}

bool ASTIfStmt::hasElseStmt() const noexcept {
    return stmts.size() > conditionExprs.size();
}

void ASTIfStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}