#include "../ir_for_stmt.hpp"

#include "../defs/ir_defs.hpp"

IRForStmt::IRForStmt() : IRStmt(IRNodeType::FOR) {}

void IRForStmt::setForSt(std::unique_ptr<IRAssignStmt> initStmt, std::unique_ptr<IRExpr> condExpr, std::unique_ptr<IRAssignStmt> incStmt, std::unique_ptr<IRStmt> statement, std::unique_ptr<IRTemporaryExpr> tempExpr){
    initializerStmt = std::move(initStmt);
    conditionExpr = std::move(condExpr);
    incrementerStmt = std::move(incStmt);
    stmt = std::move(statement);
    temporaryExpr = std::move(tempExpr);
}

bool IRForStmt::hasInitializerStmt() const noexcept {
    return initializerStmt != nullptr;
}

bool IRForStmt::hasConditionExpr() const noexcept {
    return conditionExpr != nullptr;
}

bool IRForStmt::hasIncrementerStmt() const noexcept {
    return incrementerStmt != nullptr;
}

bool IRForStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void IRForStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}