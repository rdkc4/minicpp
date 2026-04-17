#include "../ir_assign_stmt.hpp"

#include "../defs/ir_defs.hpp"

ir::IRAssignStmt::IRAssignStmt() : IRStmt(ir::IRNodeType::ASSIGN) {}

void ir::IRAssignStmt::setAssignStmt(
    std::unique_ptr<IRIdExpr> idExpr, 
    std::unique_ptr<IRExpr> expr, 
    std::unique_ptr<IRTemporaryExpr> tempExpr
){
    variableIdExpr = std::move(idExpr);
    assignedExpr = std::move(expr);
    temporaryExpr = std::move(tempExpr);
}

bool ir::IRAssignStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void ir::IRAssignStmt::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}