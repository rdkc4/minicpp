#include "../ir_switch_block_stmt.hpp"

#include "../defs/ir_defs.hpp"

IRSwitchBlockStmt::IRSwitchBlockStmt() : IRStmt(IRNodeType::SWITCH_BLOCK) {}

const std::vector<std::unique_ptr<IRStmt>>& IRSwitchBlockStmt::getStmts() const noexcept {
    return stmts;
}

void IRSwitchBlockStmt::addStmt(std::unique_ptr<IRStmt> stmt){
    stmts.push_back(std::move(stmt));
}

void IRSwitchBlockStmt::eliminateDeadStmts(size_t startIdx){
    if(startIdx < stmts.size()){
        stmts.erase(stmts.begin() + startIdx, stmts.end());
    }
}

void IRSwitchBlockStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}