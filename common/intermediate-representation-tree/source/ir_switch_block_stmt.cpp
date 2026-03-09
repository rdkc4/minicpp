#include "../ir_switch_block_stmt.hpp"

IRSwitchBlockStmt::IRSwitchBlockStmt(IRNodeType ntype) : IRStmt(ntype) {}

const std::vector<std::unique_ptr<IRStmt>>& IRSwitchBlockStmt::getStatements() const noexcept {
    return statements;
}

void IRSwitchBlockStmt::addStatement(std::unique_ptr<IRStmt> statement){
    statements.push_back(std::move(statement));
}

void IRSwitchBlockStmt::eliminateDead(size_t startIdx){
    if(startIdx < statements.size()){
        statements.erase(statements.begin() + startIdx, statements.end());
    }
}

void IRSwitchBlockStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}