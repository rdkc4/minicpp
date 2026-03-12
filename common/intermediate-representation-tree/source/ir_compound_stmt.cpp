#include "../ir_compound_stmt.hpp"

#include "../defs/ir_defs.hpp"

IRCompoundStmt::IRCompoundStmt() : IRStmt(IRNodeType::COMPOUND) {}

const std::vector<std::unique_ptr<IRStmt>>& IRCompoundStmt::getStmts() const noexcept {
    return stmts;
}

void IRCompoundStmt::addStmt(std::unique_ptr<IRStmt> stmt){
    stmts.push_back(std::move(stmt));
}

void IRCompoundStmt::eliminateDeadStmts(size_t startIdx){
    if(startIdx < stmts.size()){
        stmts.erase(stmts.begin() + startIdx, stmts.end());
    }
}

void IRCompoundStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}