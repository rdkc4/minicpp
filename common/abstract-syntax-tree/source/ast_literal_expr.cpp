#include "../ast_literal_expr.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTLiteralExpr::ASTLiteralExpr(const Token token, Type type) 
    : ASTExpr(token, AST::defs::ASTNodeType::LITERAL_EXPR, type) {}

void AST::node::ASTLiteralExpr::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}