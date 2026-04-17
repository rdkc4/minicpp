#include "../ast_switch_block_stmt.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTSwitchBlockStmt::ASTSwitchBlockStmt(const syntax::Token& token) 
    : ASTStmt(token, syntax::ast::ASTNodeType::SWITCH_BLOCK_STMT) {}

const std::vector<std::unique_ptr<syntax::ast::ASTStmt>>& 
syntax::ast::ASTSwitchBlockStmt::getStmts() const noexcept {
    return stmts;
}

void syntax::ast::ASTSwitchBlockStmt::addStmt(std::unique_ptr<syntax::ast::ASTStmt> stmt){
    stmts.push_back(std::move(stmt));
}

void syntax::ast::ASTSwitchBlockStmt::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}