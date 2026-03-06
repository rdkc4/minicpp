#include "../ir_while_stmt.hpp"

IRWhileStmt::IRWhileStmt(IRNodeType ntype) : IRStmt(ntype) {}

const IRExpr* IRWhileStmt::getCondition() const noexcept {
    return condition.get();
}

const IRStmt* IRWhileStmt::getStatement() const noexcept {
    return statement.get();
}

void IRWhileStmt::setWhileSt(std::unique_ptr<IRExpr> cond, std::unique_ptr<IRStmt> st, std::unique_ptr<IRTemporaryExpr> temp){
    condition = std::move(cond);
    statement = std::move(st);
    temporaries = std::move(temp);
}

const IRTemporaryExpr* IRWhileStmt::getTemporaries() const noexcept {
    return temporaries.get();
}

bool IRWhileStmt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}