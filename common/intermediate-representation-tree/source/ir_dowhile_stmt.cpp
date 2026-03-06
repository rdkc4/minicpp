#include "../ir_dowhile_stmt.hpp"

IRDoWhileStmt::IRDoWhileStmt(IRNodeType ntype) : IRStmt(ntype) {}

const IRExpr* IRDoWhileStmt::getCondition() const noexcept {
    return condition.get();
}

void IRDoWhileStmt::setDoWhileSt(std::unique_ptr<IRExpr> cond, std::unique_ptr<IRStmt> st, std::unique_ptr<IRTemporaryExpr> temp){
    condition = std::move(cond);
    statement = std::move(st);
    temporaries = std::move(temp);
}

const IRTemporaryExpr* IRDoWhileStmt::getTemporaries() const noexcept {
    return temporaries.get();
}

bool IRDoWhileStmt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}