#include "../ir_compound_stmt.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRCompoundStmt::IRCompoundStmt() : IRStmt(IR::defs::IRNodeType::COMPOUND) {}

const std::vector<std::unique_ptr<IR::node::IRStmt>>& 
IR::node::IRCompoundStmt::getStmts() const noexcept {
    return stmts;
}

void IR::node::IRCompoundStmt::addStmt(std::unique_ptr<IRStmt> stmt){
    stmts.push_back(std::move(stmt));
}

void IR::node::IRCompoundStmt::eliminateDeadStmts(size_t startIdx){
    if(startIdx < stmts.size()){
        stmts.erase(stmts.begin() + startIdx, stmts.end());
    }
}

void IR::node::IRCompoundStmt::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}