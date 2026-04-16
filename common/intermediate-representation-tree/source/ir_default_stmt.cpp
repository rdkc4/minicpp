#include "../ir_default_stmt.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRDefaultStmt::IRDefaultStmt() 
    : IRStmt(IR::defs::IRNodeType::DEFAULT) {}

void IR::node::IRDefaultStmt::setSwitchBlock(std::unique_ptr<IRSwitchBlockStmt> swBlockStmt){
    switchBlockStmt = std::move(swBlockStmt);
}

void IR::node::IRDefaultStmt::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}