#include "../ir_default_stmt.hpp"

#include "../defs/ir_defs.hpp"

ir::IRDefaultStmt::IRDefaultStmt() 
    : IRStmt(ir::IRNodeType::DEFAULT) {}

void ir::IRDefaultStmt::setSwitchBlock(std::unique_ptr<IRSwitchBlockStmt> swBlockStmt){
    switchBlockStmt = std::move(swBlockStmt);
}

void ir::IRDefaultStmt::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}