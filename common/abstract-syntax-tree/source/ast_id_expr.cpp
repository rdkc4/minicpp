#include "../ast_id_expr.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTIdExpr::ASTIdExpr(const Token token, Type type) 
    : ASTExpr(token, AST::defs::ASTNodeType::ID_EXPR, type) {}

void AST::node::ASTIdExpr::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}