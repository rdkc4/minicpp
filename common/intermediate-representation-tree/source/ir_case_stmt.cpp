#include "../ir_case_stmt.hpp"

IRCaseStmt::IRCaseStmt(IRNodeType ntype) : IRStmt(ntype) {}

void IRCaseStmt::setCase(std::unique_ptr<IRLiteralExpr> lit, std::unique_ptr<IRSwitchBlockStmt> block, bool hasBr){
    literal = std::move(lit);
    swBlock = std::move(block);
    breaks = hasBr;
}

bool IRCaseStmt::hasBreak() const noexcept {
    return breaks;
}

void IRCaseStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}