#include "../ir_for_stmt.hpp"

#include "../defs/ir_defs.hpp"

ir::IRForStmt::IRForStmt() : IRStmt(ir::IRNodeType::FOR) {}

void ir::IRForStmt::setForStmt(
    std::unique_ptr<IRAssignStmt> initStmt, 
    std::unique_ptr<IRExpr> condExpr, 
    std::unique_ptr<IRAssignStmt> incStmt, 
    std::unique_ptr<IRStmt> statement, 
    std::unique_ptr<IRTemporaryExpr> tempExpr
){
    initializerStmt = std::move(initStmt);
    conditionExpr = std::move(condExpr);
    incrementerStmt = std::move(incStmt);
    stmt = std::move(statement);
    temporaryExpr = std::move(tempExpr);
}

bool ir::IRForStmt::hasInitializerStmt() const noexcept {
    return initializerStmt != nullptr;
}

bool ir::IRForStmt::hasConditionExpr() const noexcept {
    return conditionExpr != nullptr;
}

bool ir::IRForStmt::hasIncrementerStmt() const noexcept {
    return incrementerStmt != nullptr;
}

bool ir::IRForStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void ir::IRForStmt::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}