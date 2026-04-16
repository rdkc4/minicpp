#include "../ir_assign_stmt.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRAssignStmt::IRAssignStmt() : IRStmt(IR::defs::IRNodeType::ASSIGN) {}

void IR::node::IRAssignStmt::setAssignStmt(
    std::unique_ptr<IRIdExpr> idExpr, 
    std::unique_ptr<IRExpr> expr, 
    std::unique_ptr<IRTemporaryExpr> tempExpr
){
    variableIdExpr = std::move(idExpr);
    assignedExpr = std::move(expr);
    temporaryExpr = std::move(tempExpr);
}

bool IR::node::IRAssignStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void IR::node::IRAssignStmt::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}