#include "../ir_switch_block_stmt.hpp"

#include "../defs/ir_defs.hpp"

ir::IRSwitchBlockStmt::IRSwitchBlockStmt() 
    : IRStmt(ir::IRNodeType::SWITCH_BLOCK) {}

const std::vector<std::unique_ptr<ir::IRStmt>>& 
ir::IRSwitchBlockStmt::getStmts() const noexcept {
    return stmts;
}

void ir::IRSwitchBlockStmt::addStmt(std::unique_ptr<IRStmt> stmt){
    stmts.push_back(std::move(stmt));
}

void ir::IRSwitchBlockStmt::eliminateDeadStmts(size_t startIdx){
    if(startIdx < stmts.size()){
        stmts.erase(stmts.begin() + startIdx, stmts.end());
    }
}

void ir::IRSwitchBlockStmt::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}