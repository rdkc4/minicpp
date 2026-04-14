#include "../ir_return_stmt.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRReturnStmt::IRReturnStmt() : IRStmt(IR::defs::IRNodeType::RETURN) {}

void IR::node::IRReturnStmt::setReturnExpr(std::unique_ptr<IRExpr> expr, std::unique_ptr<IRTemporaryExpr> tempExpr){
    returnExpr = std::move(expr);
    temporaryExpr = std::move(tempExpr);
}

bool IR::node::IRReturnStmt::hasReturnValue() const noexcept {
    return returnExpr != nullptr;
}

bool IR::node::IRReturnStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void IR::node::IRReturnStmt::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}