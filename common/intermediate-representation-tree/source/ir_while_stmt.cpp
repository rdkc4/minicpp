#include "../ir_while_stmt.hpp"

#include "../defs/ir_defs.hpp"

IRWhileStmt::IRWhileStmt() : IRStmt(IRNodeType::WHILE) {}

void IRWhileStmt::setWhileStmt(std::unique_ptr<IRExpr> condExpr, std::unique_ptr<IRStmt> statement, std::unique_ptr<IRTemporaryExpr> tempExpr){
    conditionExpr = std::move(condExpr);
    stmt = std::move(statement);
    temporaryExpr = std::move(tempExpr);
}

bool IRWhileStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void IRWhileStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}