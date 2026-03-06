#include "../ir_assign_stmt.hpp"

IRAssignStmt::IRAssignStmt(IRNodeType ntype) : IRStmt(ntype) {}

const IRIdExpr* IRAssignStmt::getVariable() const noexcept {
    return variable.get();
}

const IRExpr* IRAssignStmt::getExp() const noexcept {
    return exp.get();
}

const IRTemporaryExpr* IRAssignStmt::getTemporaries() const noexcept {
    return temporaries.get();
}

void IRAssignStmt::setAssignSt(std::unique_ptr<IRIdExpr> var, std::unique_ptr<IRExpr> _exp, std::unique_ptr<IRTemporaryExpr> temp){
    variable = std::move(var);
    exp = std::move(_exp);
    temporaries = std::move(temp);
}

bool IRAssignStmt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}