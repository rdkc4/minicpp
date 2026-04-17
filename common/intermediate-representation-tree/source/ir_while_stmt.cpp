#include "../ir_while_stmt.hpp"

#include "../defs/ir_defs.hpp"

ir::IRWhileStmt::IRWhileStmt() : IRStmt(ir::IRNodeType::WHILE) {}

void ir::IRWhileStmt::setWhileStmt(
    std::unique_ptr<IRExpr> condExpr, 
    std::unique_ptr<IRStmt> statement, 
    std::unique_ptr<IRTemporaryExpr> tempExpr
){
    conditionExpr = std::move(condExpr);
    stmt = std::move(statement);
    temporaryExpr = std::move(tempExpr);
}

bool ir::IRWhileStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void ir::IRWhileStmt::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}