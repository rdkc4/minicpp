#include "../ir_return_stmt.hpp"

IRReturnStmt::IRReturnStmt(IRNodeType ntype) : IRStmt(ntype) {}

void IRReturnStmt::setExp(std::unique_ptr<IRExpr> _exp, std::unique_ptr<IRTemporaryExpr> temp){
    exp = std::move(_exp);
    temporaries = std::move(temp);
}

bool IRReturnStmt::returns() const noexcept {
    return exp != nullptr;
}

bool IRReturnStmt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}

void IRReturnStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}