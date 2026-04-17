#include "../ir_case_stmt.hpp"

#include "../defs/ir_defs.hpp"

ir::IRCaseStmt::IRCaseStmt() : IRStmt(ir::IRNodeType::CASE) {}

void ir::IRCaseStmt::setCase(
    std::unique_ptr<IRLiteralExpr> litExpr, 
    std::unique_ptr<IRSwitchBlockStmt> swBlockStmt, 
    bool hasBreak
){
    literalExpr = std::move(litExpr);
    switchBlockStmt = std::move(swBlockStmt);
    breaks = hasBreak;
}

bool ir::IRCaseStmt::hasBreakStmt() const noexcept {
    return breaks;
}

void ir::IRCaseStmt::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}