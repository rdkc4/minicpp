#include "../ir_assign_stmt.hpp"

#include "../defs/ir_defs.hpp"

IRAssignStmt::IRAssignStmt() : IRStmt(IRNodeType::ASSIGN) {}

void IRAssignStmt::setAssignStmt(std::unique_ptr<IRIdExpr> idExpr, std::unique_ptr<IRExpr> expr, std::unique_ptr<IRTemporaryExpr> tempExpr){
    variableIdExpr = std::move(idExpr);
    assignedExpr = std::move(expr);
    temporaryExpr = std::move(tempExpr);
}

bool IRAssignStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void IRAssignStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}