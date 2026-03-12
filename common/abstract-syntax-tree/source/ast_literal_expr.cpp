#include "../ast_literal_expr.hpp"

#include "../defs/ast_defs.hpp"

ASTLiteralExpr::ASTLiteralExpr(const Token token, Type type) : ASTExpr(token, ASTNodeType::LITERAL, type) {}

void ASTLiteralExpr::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}