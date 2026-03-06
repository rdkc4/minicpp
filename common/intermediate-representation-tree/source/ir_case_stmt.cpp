#include "../ir_case_stmt.hpp"

IRCaseStmt::IRCaseStmt(IRNodeType ntype) : IRStmt(ntype) {}

const IRLiteralExpr* IRCaseStmt::getLiteral() const noexcept {
    return literal.get();
}

void IRCaseStmt::setCase(std::unique_ptr<IRLiteralExpr> lit, std::unique_ptr<IRSwitchBlockStmt> block, bool hasBr){
    literal = std::move(lit);
    swBlock = std::move(block);
    breaks = hasBr;
}

bool IRCaseStmt::hasBreak() const noexcept {
    return breaks;
}