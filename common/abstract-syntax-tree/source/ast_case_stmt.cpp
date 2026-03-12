#include "../ast_case_stmt.hpp"

#include "../defs/ast_defs.hpp"

ASTCaseStmt::ASTCaseStmt(const Token& token) : ASTStmt(token, ASTNodeType::CASE) {}

void ASTCaseStmt::setCase(std::unique_ptr<ASTLiteralExpr> litExpr, std::unique_ptr<ASTSwitchBlockStmt> swBlockStmt, bool hasBreak){
    literalExpr = std::move(litExpr);
    switchBlockStmt = std::move(swBlockStmt);
    breaks = hasBreak;
}

bool ASTCaseStmt::hasBreakStmt() const noexcept {
    return breaks;
}

void ASTCaseStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}