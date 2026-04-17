#include "../ir_compound_stmt.hpp"

#include "../defs/ir_defs.hpp"

ir::IRCompoundStmt::IRCompoundStmt() : IRStmt(ir::IRNodeType::COMPOUND) {}

const std::vector<std::unique_ptr<ir::IRStmt>>& 
ir::IRCompoundStmt::getStmts() const noexcept {
    return stmts;
}

void ir::IRCompoundStmt::addStmt(std::unique_ptr<IRStmt> stmt){
    stmts.push_back(std::move(stmt));
}

void ir::IRCompoundStmt::eliminateDeadStmts(size_t startIdx){
    if(startIdx < stmts.size()){
        stmts.erase(stmts.begin() + startIdx, stmts.end());
    }
}

void ir::IRCompoundStmt::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}