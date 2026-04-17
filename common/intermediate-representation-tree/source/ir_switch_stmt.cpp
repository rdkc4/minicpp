#include "../ir_switch_stmt.hpp"

#include "../defs/ir_defs.hpp"

ir::IRSwitchStmt::IRSwitchStmt() 
    : IRStmt(ir::IRNodeType::SWITCH) {}

void ir::IRSwitchStmt::setVariableIdExpr(std::unique_ptr<IRIdExpr> idExpr){
    variableIdExpr = std::move(idExpr);
}

const std::vector<std::unique_ptr<ir::IRCaseStmt>>& 
ir::IRSwitchStmt::getCaseStmts() const noexcept {
    return caseStmts;
}

const ir::IRCaseStmt* 
ir::IRSwitchStmt::getCaseStmtAtN(size_t n) const noexcept {
    return caseStmts[n].get();
}

void ir::IRSwitchStmt::addCaseStmt(std::unique_ptr<IRCaseStmt> caseStmt){
    caseStmts.push_back(std::move(caseStmt));
}

void ir::IRSwitchStmt::setDefaultStmt(std::unique_ptr<IRDefaultStmt> swDefaultStmt) {
    defaultStmt = std::move(swDefaultStmt);
}

bool ir::IRSwitchStmt::hasDefaultStmt() const noexcept {
    return defaultStmt != nullptr;
}

size_t ir::IRSwitchStmt::getCaseCount() const noexcept {
    return caseStmts.size();
}

void ir::IRSwitchStmt::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}