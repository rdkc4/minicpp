#include "../ir_for_stmt.hpp"

IRForStmt::IRForStmt(IRNodeType ntype) : IRStmt(ntype) {}

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

bool IRForStmt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}

void IRForStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}