#include "../ir_switch_block_stmt.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRSwitchBlockStmt::IRSwitchBlockStmt() 
    : IRStmt(IR::defs::IRNodeType::SWITCH_BLOCK) {}

const std::vector<std::unique_ptr<IR::node::IRStmt>>& 
IR::node::IRSwitchBlockStmt::getStmts() const noexcept {
    return stmts;
}

void IR::node::IRSwitchBlockStmt::addStmt(std::unique_ptr<IRStmt> stmt){
    stmts.push_back(std::move(stmt));
}

void IR::node::IRSwitchBlockStmt::eliminateDeadStmts(size_t startIdx){
    if(startIdx < stmts.size()){
        stmts.erase(stmts.begin() + startIdx, stmts.end());
    }
}

void IR::node::IRSwitchBlockStmt::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}