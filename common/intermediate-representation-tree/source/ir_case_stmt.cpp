#include "../ir_case_stmt.hpp"

#include "../defs/ir_defs.hpp"

IRCaseStmt::IRCaseStmt() : IRStmt(IRNodeType::CASE) {}

void IRCaseStmt::setCase(std::unique_ptr<IRLiteralExpr> litExpr, std::unique_ptr<IRSwitchBlockStmt> swBlockStmt, bool hasBreak){
    literalExpr = std::move(litExpr);
    switchBlockStmt = std::move(swBlockStmt);
    breaks = hasBreak;
}

bool IRCaseStmt::hasBreakStmt() const noexcept {
    return breaks;
}

void IRCaseStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}