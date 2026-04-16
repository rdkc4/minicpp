#include "../ast_assign_stmt.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTAssignStmt::ASTAssignStmt(const Token& token) 
    : ASTStmt(token, AST::defs::ASTNodeType::ASSIGN_STMT) {}

void AST::node::ASTAssignStmt::setVariableIdExpr(std::unique_ptr<AST::node::ASTIdExpr> idExpr){
    variableIdExpr = std::move(idExpr);
}

void AST::node::ASTAssignStmt::setAssignedExpr(std::unique_ptr<AST::node::ASTExpr> expr){
    assignedExpr = std::move(expr);
}

void AST::node::ASTAssignStmt::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}