#include "../ir_if_stmt.hpp"

#include "../defs/ir_defs.hpp"

IRIfStmt::IRIfStmt() : IRStmt(IRNodeType::IF) {}

const std::vector<std::unique_ptr<IRExpr>>& IRIfStmt::getConditionExprs() const noexcept {
    return conditionExprs;
}

const std::vector<std::unique_ptr<IRStmt>>& IRIfStmt::getStmts() const noexcept {
    return stmts;
}

const std::vector<std::unique_ptr<IRTemporaryExpr>>& IRIfStmt::getTemporaryExprs() const noexcept {
    return temporaryExprs;
}

size_t IRIfStmt::getConditionCount() const noexcept {
    return conditionExprs.size();
}

void IRIfStmt::addIfStmt(std::unique_ptr<IRExpr> condExpr, std::unique_ptr<IRStmt> statement, std::unique_ptr<IRTemporaryExpr> tempExpr){
    conditionExprs.push_back(std::move(condExpr));
    stmts.push_back(std::move(statement));
    temporaryExprs.push_back(std::move(tempExpr));
}

void IRIfStmt::addElseStmt(std::unique_ptr<IRStmt> statement){
    stmts.push_back(std::move(statement));
}

bool IRIfStmt::hasElseStmt() const noexcept {
    return stmts.size() > conditionExprs.size();
}

const IRStmt* IRIfStmt::getElseStmt() const noexcept {
    return hasElseStmt() ? stmts.back().get() : nullptr;
}

const std::tuple<const IRExpr*, const IRStmt*, const IRTemporaryExpr*> IRIfStmt::getIfStmtAtN(size_t n) const noexcept {
    return {conditionExprs[n].get(), stmts[n].get(), temporaryExprs[n].get() };
}

void IRIfStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}