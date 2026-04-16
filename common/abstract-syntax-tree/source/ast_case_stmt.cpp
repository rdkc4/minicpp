#include "../ast_case_stmt.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTCaseStmt::ASTCaseStmt(const Token& token) 
    : ASTStmt(token, AST::defs::ASTNodeType::CASE_STMT) {}

void AST::node::ASTCaseStmt::setCase(
    std::unique_ptr<AST::node::ASTLiteralExpr> litExpr, 
    std::unique_ptr<AST::node::ASTSwitchBlockStmt> swBlockStmt, 
    bool hasBreak
){
    literalExpr = std::move(litExpr);
    switchBlockStmt = std::move(swBlockStmt);
    breaks = hasBreak;
}

bool AST::node::ASTCaseStmt::hasBreakStmt() const noexcept {
    return breaks;
}

void AST::node::ASTCaseStmt::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}