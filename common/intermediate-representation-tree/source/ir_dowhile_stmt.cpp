#include "../ir_dowhile_stmt.hpp"

IRDoWhileStmt::IRDoWhileStmt(IRNodeType ntype) : IRStmt(ntype) {}

void IRDoWhileStmt::setDoWhileSt(std::unique_ptr<IRExpr> cond, std::unique_ptr<IRStmt> st, std::unique_ptr<IRTemporaryExpr> temp){
    condition = std::move(cond);
    statement = std::move(st);
    temporaries = std::move(temp);
}

bool IRDoWhileStmt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}

void IRDoWhileStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}