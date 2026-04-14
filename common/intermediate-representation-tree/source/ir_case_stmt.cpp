#include "../ir_case_stmt.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRCaseStmt::IRCaseStmt() : IRStmt(IR::defs::IRNodeType::CASE) {}

void IR::node::IRCaseStmt::setCase(std::unique_ptr<IRLiteralExpr> litExpr, std::unique_ptr<IRSwitchBlockStmt> swBlockStmt, bool hasBreak){
    literalExpr = std::move(litExpr);
    switchBlockStmt = std::move(swBlockStmt);
    breaks = hasBreak;
}

bool IR::node::IRCaseStmt::hasBreakStmt() const noexcept {
    return breaks;
}

void IR::node::IRCaseStmt::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}