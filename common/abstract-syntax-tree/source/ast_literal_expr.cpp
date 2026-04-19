#include "../ast_literal_expr.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTLiteralExpr::ASTLiteralExpr(const syntax::Token& token, types::Type type) 
    : ASTExpr(token, syntax::ast::ASTNodeType::LITERAL_EXPR, type) {}

void syntax::ast::ASTLiteralExpr::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}