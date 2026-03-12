#include "../ir_dowhile_stmt.hpp"

#include "../defs/ir_defs.hpp"

IRDoWhileStmt::IRDoWhileStmt() : IRStmt(IRNodeType::DO_WHILE) {}

void IRDoWhileStmt::setDoWhileStmt(std::unique_ptr<IRExpr> condExpr, std::unique_ptr<IRStmt> statement, std::unique_ptr<IRTemporaryExpr> tempExpr){
    conditionExpr = std::move(condExpr);
    stmt = std::move(statement);
    temporaryExpr = std::move(tempExpr);
}

bool IRDoWhileStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void IRDoWhileStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}