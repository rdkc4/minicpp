#include "../ast_while_stmt.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTWhileStmt::ASTWhileStmt(const Token& token) 
    : ASTStmt(token, AST::defs::ASTNodeType::WHILE_STMT) {}

void AST::node::ASTWhileStmt::setWhileStmt(
    std::unique_ptr<AST::node::ASTExpr> cond, 
    std::unique_ptr<AST::node::ASTStmt> statement
){
    conditionExpr = std::move(cond);
    stmt = std::move(statement);
}

void AST::node::ASTWhileStmt::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}