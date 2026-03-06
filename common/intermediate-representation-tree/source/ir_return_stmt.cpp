#include "../ir_return_stmt.hpp"

IRReturnStmt::IRReturnStmt(IRNodeType ntype) : IRStmt(ntype) {}

const IRExpr* IRReturnStmt::getExp() const noexcept {
    return exp.get();
}

void IRReturnStmt::setExp(std::unique_ptr<IRExpr> _exp, std::unique_ptr<IRTemporaryExpr> temp){
    exp = std::move(_exp);
    temporaries = std::move(temp);
}

const IRTemporaryExpr* IRReturnStmt::getTemporaries() const noexcept {
    return temporaries.get();
}

bool IRReturnStmt::returns() const noexcept {
    return exp != nullptr;
}

bool IRReturnStmt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}