#include "../ir_for_stmt.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRForStmt::IRForStmt() : IRStmt(IR::defs::IRNodeType::FOR) {}

void IR::node::IRForStmt::setForSt(std::unique_ptr<IRAssignStmt> initStmt, std::unique_ptr<IRExpr> condExpr, std::unique_ptr<IRAssignStmt> incStmt, std::unique_ptr<IRStmt> statement, std::unique_ptr<IRTemporaryExpr> tempExpr){
    initializerStmt = std::move(initStmt);
    conditionExpr = std::move(condExpr);
    incrementerStmt = std::move(incStmt);
    stmt = std::move(statement);
    temporaryExpr = std::move(tempExpr);
}

bool IR::node::IRForStmt::hasInitializerStmt() const noexcept {
    return initializerStmt != nullptr;
}

bool IR::node::IRForStmt::hasConditionExpr() const noexcept {
    return conditionExpr != nullptr;
}

bool IR::node::IRForStmt::hasIncrementerStmt() const noexcept {
    return incrementerStmt != nullptr;
}

bool IR::node::IRForStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void IR::node::IRForStmt::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}