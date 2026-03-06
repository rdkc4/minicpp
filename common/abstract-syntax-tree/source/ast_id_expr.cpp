#include "../ast_id_expr.hpp"

ASTIdExpr::ASTIdExpr(const Token token, ASTNodeType ntype, Type type) : ASTExpr(token, ntype, type) {}

void ASTIdExpr::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}