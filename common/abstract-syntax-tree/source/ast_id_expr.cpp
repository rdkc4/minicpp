#include "../ast_id_expr.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTIdExpr::ASTIdExpr(const syntax::Token& token, types::Type type) 
    : ASTExpr(token, syntax::ast::ASTNodeType::ID_EXPR, type) {}

void syntax::ast::ASTIdExpr::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}