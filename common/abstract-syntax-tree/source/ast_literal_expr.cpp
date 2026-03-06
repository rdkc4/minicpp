#include "../ast_literal_expr.hpp"

ASTLiteralExpr::ASTLiteralExpr(const Token token, ASTNodeType ntype, Type type) : ASTExpr(token, ntype, type) {}

void ASTLiteralExpr::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}