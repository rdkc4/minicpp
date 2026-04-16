#include "../ir_if_stmt.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRIfStmt::IRIfStmt() : IRStmt(IR::defs::IRNodeType::IF) {}

const std::vector<std::unique_ptr<IR::node::IRExpr>>& 
IR::node::IRIfStmt::getConditionExprs() const noexcept {
    return conditionExprs;
}

const std::vector<std::unique_ptr<IR::node::IRStmt>>& 
IR::node::IRIfStmt::getStmts() const noexcept {
    return stmts;
}

const std::vector<std::unique_ptr<IR::node::IRTemporaryExpr>>& 
IR::node::IRIfStmt::getTemporaryExprs() const noexcept {
    return temporaryExprs;
}

size_t IR::node::IRIfStmt::getConditionCount() const noexcept {
    return conditionExprs.size();
}

void IR::node::IRIfStmt::addIfStmt(
    std::unique_ptr<IRExpr> condExpr, 
    std::unique_ptr<IRStmt> statement, 
    std::unique_ptr<IRTemporaryExpr> tempExpr
){
    conditionExprs.push_back(std::move(condExpr));
    stmts.push_back(std::move(statement));
    temporaryExprs.push_back(std::move(tempExpr));
}

void IR::node::IRIfStmt::addElseStmt(std::unique_ptr<IRStmt> statement){
    stmts.push_back(std::move(statement));
}

bool IR::node::IRIfStmt::hasElseStmt() const noexcept {
    return stmts.size() > conditionExprs.size();
}

const IR::node::IRStmt* IR::node::IRIfStmt::getElseStmt() const noexcept {
    return hasElseStmt() ? stmts.back().get() : nullptr;
}

const std::tuple<const IR::node::IRExpr*, const IR::node::IRStmt*, const IR::node::IRTemporaryExpr*> 
IR::node::IRIfStmt::getIfStmtAtN(size_t n) const noexcept {
    return {conditionExprs[n].get(), stmts[n].get(), temporaryExprs[n].get() };
}

void IR::node::IRIfStmt::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}