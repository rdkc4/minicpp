#include "../ir_assign_stmt.hpp"

IRAssignStmt::IRAssignStmt(IRNodeType ntype) : IRStmt(ntype) {}

void IRAssignStmt::setAssignSt(std::unique_ptr<IRIdExpr> var, std::unique_ptr<IRExpr> _exp, std::unique_ptr<IRTemporaryExpr> temp){
    variable = std::move(var);
    exp = std::move(_exp);
    temporaries = std::move(temp);
}

bool IRAssignStmt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}

void IRAssignStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}