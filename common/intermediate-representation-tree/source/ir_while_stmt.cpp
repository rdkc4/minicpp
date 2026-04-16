#include "../ir_while_stmt.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRWhileStmt::IRWhileStmt() : IRStmt(IR::defs::IRNodeType::WHILE) {}

void IR::node::IRWhileStmt::setWhileStmt(
    std::unique_ptr<IRExpr> condExpr, 
    std::unique_ptr<IRStmt> statement, 
    std::unique_ptr<IRTemporaryExpr> tempExpr
){
    conditionExpr = std::move(condExpr);
    stmt = std::move(statement);
    temporaryExpr = std::move(tempExpr);
}

bool IR::node::IRWhileStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void IR::node::IRWhileStmt::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}