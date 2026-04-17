#include "../ast_default_stmt.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTDefaultStmt::ASTDefaultStmt(const syntax::Token& token) 
    : ASTStmt(token, syntax::ast::ASTNodeType::DEFAULT_STMT) {}

void syntax::ast::ASTDefaultStmt::setDefaultStmt(std::unique_ptr<syntax::ast::ASTSwitchBlockStmt> swBlockStmt){
    switchBlockStmt = std::move(swBlockStmt);
}

void syntax::ast::ASTDefaultStmt::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}