#include "../ir_switch_stmt.hpp"

IRSwitchStmt::IRSwitchStmt(IRNodeType ntype) : IRStmt(ntype) {}

void IRSwitchStmt::setVariable(std::unique_ptr<IRIdExpr> var){
    variable = std::move(var);
}

const std::vector<std::unique_ptr<IRCaseStmt>>& IRSwitchStmt::getCases() const noexcept {
    return cases;
}

const IRCaseStmt* IRSwitchStmt::getCaseAtN(size_t n) const noexcept {
    return cases[n].get();
}

void IRSwitchStmt::addCase(std::unique_ptr<IRCaseStmt> _case){
    cases.push_back(std::move(_case));
}

void IRSwitchStmt::setDefault(std::unique_ptr<IRDefaultStmt> _swDefault) {
    _default = std::move(_swDefault);
}

bool IRSwitchStmt::hasDefault() const noexcept {
    return _default != nullptr;
}

size_t IRSwitchStmt::getCaseCount() const noexcept {
    return cases.size();
}

void IRSwitchStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}