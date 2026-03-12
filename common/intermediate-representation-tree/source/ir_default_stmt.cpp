#include "../ir_default_stmt.hpp"

#include "../defs/ir_defs.hpp"

IRDefaultStmt::IRDefaultStmt() : IRStmt(IRNodeType::DEFAULT) {}

void IRDefaultStmt::setSwitchBlock(std::unique_ptr<IRSwitchBlockStmt> swBlockStmt){
    switchBlockStmt = std::move(swBlockStmt);
}

void IRDefaultStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}