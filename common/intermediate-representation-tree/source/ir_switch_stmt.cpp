#include "../ir_switch_stmt.hpp"

#include "../defs/ir_defs.hpp"

IRSwitchStmt::IRSwitchStmt() : IRStmt(IRNodeType::SWITCH) {}

void IRSwitchStmt::setVariableIdExpr(std::unique_ptr<IRIdExpr> idExpr){
    variableIdExpr = std::move(idExpr);
}

const std::vector<std::unique_ptr<IRCaseStmt>>& IRSwitchStmt::getCaseStmts() const noexcept {
    return caseStmts;
}

const IRCaseStmt* IRSwitchStmt::getCaseStmtAtN(size_t n) const noexcept {
    return caseStmts[n].get();
}

void IRSwitchStmt::addCaseStmt(std::unique_ptr<IRCaseStmt> caseStmt){
    caseStmts.push_back(std::move(caseStmt));
}

void IRSwitchStmt::setDefaultStmt(std::unique_ptr<IRDefaultStmt> swDefaultStmt) {
    defaultStmt = std::move(swDefaultStmt);
}

bool IRSwitchStmt::hasDefaultStmt() const noexcept {
    return defaultStmt != nullptr;
}

size_t IRSwitchStmt::getCaseCount() const noexcept {
    return caseStmts.size();
}

void IRSwitchStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}