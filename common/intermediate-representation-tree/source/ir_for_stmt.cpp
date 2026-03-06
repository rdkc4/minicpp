#include "../ir_for_stmt.hpp"

IRForStmt::IRForStmt(IRNodeType ntype) : IRStmt(ntype) {}

const IRAssignStmt* IRForStmt::getInitializer() const noexcept {
    return initializer.get();
}

const IRExpr* IRForStmt::getCondition() const noexcept {
    return condition.get();
}

const IRAssignStmt* IRForStmt::getIncrementer() const noexcept {
    return incrementer.get();
}

const IRStmt* IRForStmt::getStatement() const noexcept {
    return statement.get();
}

void IRForStmt::setForSt(std::unique_ptr<IRAssignStmt> init, std::unique_ptr<IRExpr> cond, std::unique_ptr<IRAssignStmt> inc, std::unique_ptr<IRStmt> st, std::unique_ptr<IRTemporaryExpr> temp){
    initializer = std::move(init);
    condition = std::move(cond);
    incrementer = std::move(inc);
    statement = std::move(st);
    temporaries = std::move(temp);
}

bool IRForStmt::hasInitializer() const noexcept {
    return initializer != nullptr;
}

bool IRForStmt::hasCondition() const noexcept {
    return condition != nullptr;
}

bool IRForStmt::hasIncrementer() const noexcept {
    return incrementer != nullptr;
}

const IRTemporaryExpr* IRForStmt::getTemporaries() const noexcept {
    return temporaries.get();
}

bool IRForStmt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}