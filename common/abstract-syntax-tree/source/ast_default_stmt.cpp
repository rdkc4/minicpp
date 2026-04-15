#include "../ast_default_stmt.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTDefaultStmt::ASTDefaultStmt(const Token& token) 
    : ASTStmt(token, AST::defs::ASTNodeType::DEFAULT_STMT) {}

void AST::node::ASTDefaultStmt::setDefaultStmt(std::unique_ptr<AST::node::ASTSwitchBlockStmt> swBlockStmt){
    switchBlockStmt = std::move(swBlockStmt);
}

void AST::node::ASTDefaultStmt::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}