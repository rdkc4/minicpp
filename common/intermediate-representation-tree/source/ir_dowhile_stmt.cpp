#include "../ir_dowhile_stmt.hpp"

#include "../defs/ir_defs.hpp"

ir::IRDoWhileStmt::IRDoWhileStmt() 
    : IRStmt(ir::IRNodeType::DO_WHILE) {}

void ir::IRDoWhileStmt::setDoWhileStmt(
    std::unique_ptr<IRExpr> condExpr, 
    std::unique_ptr<IRStmt> statement, 
    std::unique_ptr<IRTemporaryExpr> tempExpr
){
    conditionExpr = std::move(condExpr);
    stmt = std::move(statement);
    temporaryExpr = std::move(tempExpr);
}

bool ir::IRDoWhileStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void ir::IRDoWhileStmt::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}