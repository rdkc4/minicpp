#include "../ir_return_stmt.hpp"

#include "../defs/ir_defs.hpp"

IRReturnStmt::IRReturnStmt() : IRStmt(IRNodeType::RETURN) {}

void IRReturnStmt::setReturnExpr(std::unique_ptr<IRExpr> expr, std::unique_ptr<IRTemporaryExpr> tempExpr){
    returnExpr = std::move(expr);
    temporaryExpr = std::move(tempExpr);
}

bool IRReturnStmt::hasReturnValue() const noexcept {
    return returnExpr != nullptr;
}

bool IRReturnStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void IRReturnStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}