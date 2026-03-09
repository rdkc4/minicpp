#include "../ir_default_stmt.hpp"

IRDefaultStmt::IRDefaultStmt(IRNodeType ntype) : IRStmt(ntype) {}

void IRDefaultStmt::setSwitchBlock(std::unique_ptr<IRSwitchBlockStmt> block){
    swBlock = std::move(block);
}

void IRDefaultStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}