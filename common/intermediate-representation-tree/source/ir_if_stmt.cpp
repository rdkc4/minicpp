#include "../ir_if_stmt.hpp"

#include "../defs/ir_defs.hpp"

ir::IRIfStmt::IRIfStmt() : IRStmt(ir::IRNodeType::IF) {}

const std::vector<std::unique_ptr<ir::IRExpr>>& 
ir::IRIfStmt::getConditionExprs() const noexcept {
    return conditionExprs;
}

const std::vector<std::unique_ptr<ir::IRStmt>>& 
ir::IRIfStmt::getStmts() const noexcept {
    return stmts;
}

const std::vector<std::unique_ptr<ir::IRTemporaryExpr>>& 
ir::IRIfStmt::getTemporaryExprs() const noexcept {
    return temporaryExprs;
}

size_t ir::IRIfStmt::getConditionCount() const noexcept {
    return conditionExprs.size();
}

void ir::IRIfStmt::addIfStmt(
    std::unique_ptr<IRExpr> condExpr, 
    std::unique_ptr<IRStmt> statement, 
    std::unique_ptr<IRTemporaryExpr> tempExpr
){
    conditionExprs.push_back(std::move(condExpr));
    stmts.push_back(std::move(statement));
    temporaryExprs.push_back(std::move(tempExpr));
}

void ir::IRIfStmt::addElseStmt(std::unique_ptr<IRStmt> statement){
    stmts.push_back(std::move(statement));
}

bool ir::IRIfStmt::hasElseStmt() const noexcept {
    return stmts.size() > conditionExprs.size();
}

const ir::IRStmt* ir::IRIfStmt::getElseStmt() const noexcept {
    return hasElseStmt() ? stmts.back().get() : nullptr;
}

const std::tuple<const ir::IRExpr*, const ir::IRStmt*, const ir::IRTemporaryExpr*> 
ir::IRIfStmt::getIfStmtAtN(size_t n) const noexcept {
    return {conditionExprs[n].get(), stmts[n].get(), temporaryExprs[n].get() };
}

void ir::IRIfStmt::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}