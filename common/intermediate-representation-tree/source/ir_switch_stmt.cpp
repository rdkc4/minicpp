#include "../ir_switch_stmt.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRSwitchStmt::IRSwitchStmt() : IRStmt(IR::defs::IRNodeType::SWITCH) {}

void IR::node::IRSwitchStmt::setVariableIdExpr(std::unique_ptr<IRIdExpr> idExpr){
    variableIdExpr = std::move(idExpr);
}

const std::vector<std::unique_ptr<IR::node::IRCaseStmt>>& IR::node::IRSwitchStmt::getCaseStmts() const noexcept {
    return caseStmts;
}

const IR::node::IRCaseStmt* IR::node::IRSwitchStmt::getCaseStmtAtN(size_t n) const noexcept {
    return caseStmts[n].get();
}

void IR::node::IRSwitchStmt::addCaseStmt(std::unique_ptr<IRCaseStmt> caseStmt){
    caseStmts.push_back(std::move(caseStmt));
}

void IR::node::IRSwitchStmt::setDefaultStmt(std::unique_ptr<IRDefaultStmt> swDefaultStmt) {
    defaultStmt = std::move(swDefaultStmt);
}

bool IR::node::IRSwitchStmt::hasDefaultStmt() const noexcept {
    return defaultStmt != nullptr;
}

size_t IR::node::IRSwitchStmt::getCaseCount() const noexcept {
    return caseStmts.size();
}

void IR::node::IRSwitchStmt::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}