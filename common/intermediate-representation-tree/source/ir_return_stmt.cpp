#include "../ir_return_stmt.hpp"

#include "../defs/ir_defs.hpp"

ir::IRReturnStmt::IRReturnStmt() 
    : IRStmt(ir::IRNodeType::RETURN) {}

void ir::IRReturnStmt::setReturnExpr(
    std::unique_ptr<IRExpr> expr, 
    std::unique_ptr<IRTemporaryExpr> tempExpr
){
    returnExpr = std::move(expr);
    temporaryExpr = std::move(tempExpr);
}

bool ir::IRReturnStmt::hasReturnValue() const noexcept {
    return returnExpr != nullptr;
}

bool ir::IRReturnStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void ir::IRReturnStmt::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}