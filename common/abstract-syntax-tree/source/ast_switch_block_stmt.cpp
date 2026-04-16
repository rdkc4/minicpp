#include "../ast_switch_block_stmt.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTSwitchBlockStmt::ASTSwitchBlockStmt(const Token& token) 
    : ASTStmt(token, AST::defs::ASTNodeType::SWITCH_BLOCK_STMT) {}

const std::vector<std::unique_ptr<AST::node::ASTStmt>>& 
AST::node::ASTSwitchBlockStmt::getStmts() const noexcept {
    return stmts;
}

void AST::node::ASTSwitchBlockStmt::addStmt(std::unique_ptr<AST::node::ASTStmt> stmt){
    stmts.push_back(std::move(stmt));
}

void AST::node::ASTSwitchBlockStmt::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}