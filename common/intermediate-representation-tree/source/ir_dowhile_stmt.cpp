#include "../ir_dowhile_stmt.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRDoWhileStmt::IRDoWhileStmt() : IRStmt(IR::defs::IRNodeType::DO_WHILE) {}

void IR::node::IRDoWhileStmt::setDoWhileStmt(std::unique_ptr<IRExpr> condExpr, std::unique_ptr<IRStmt> statement, std::unique_ptr<IRTemporaryExpr> tempExpr){
    conditionExpr = std::move(condExpr);
    stmt = std::move(statement);
    temporaryExpr = std::move(tempExpr);
}

bool IR::node::IRDoWhileStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void IR::node::IRDoWhileStmt::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}