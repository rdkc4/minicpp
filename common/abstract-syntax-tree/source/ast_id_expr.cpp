#include "../ast_id_expr.hpp"

#include "../defs/ast_defs.hpp"

ASTIdExpr::ASTIdExpr(const Token token, Type type) : ASTExpr(token, ASTNodeType::ID, type) {}

void ASTIdExpr::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}