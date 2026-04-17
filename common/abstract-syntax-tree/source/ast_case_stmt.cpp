#include "../ast_case_stmt.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTCaseStmt::ASTCaseStmt(const syntax::Token& token) 
    : ASTStmt(token, syntax::ast::ASTNodeType::CASE_STMT) {}

void syntax::ast::ASTCaseStmt::setCase(
    std::unique_ptr<syntax::ast::ASTLiteralExpr> litExpr, 
    std::unique_ptr<syntax::ast::ASTSwitchBlockStmt> swBlockStmt, 
    bool hasBreak
){
    literalExpr = std::move(litExpr);
    switchBlockStmt = std::move(swBlockStmt);
    breaks = hasBreak;
}

bool syntax::ast::ASTCaseStmt::hasBreakStmt() const noexcept {
    return breaks;
}

void syntax::ast::ASTCaseStmt::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}