#include "../ast_dowhile_stmt.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTDoWhileStmt::ASTDoWhileStmt(const Token& token) 
    : ASTStmt(token, AST::defs::ASTNodeType::DOWHILE_STMT) {}

void AST::node::ASTDoWhileStmt::setDoWhile(
    std::unique_ptr<AST::node::ASTExpr> condExpr, 
    std::unique_ptr<AST::node::ASTStmt> statement
){
    conditionExpr = std::move(condExpr);
    stmt = std::move(statement);
}

void AST::node::ASTDoWhileStmt::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}