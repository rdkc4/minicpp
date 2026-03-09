#include "../ir_while_stmt.hpp"

IRWhileStmt::IRWhileStmt(IRNodeType ntype) : IRStmt(ntype) {}

void IRWhileStmt::setWhileSt(std::unique_ptr<IRExpr> cond, std::unique_ptr<IRStmt> st, std::unique_ptr<IRTemporaryExpr> temp){
    condition = std::move(cond);
    statement = std::move(st);
    temporaries = std::move(temp);
}

bool IRWhileStmt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}

void IRWhileStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}