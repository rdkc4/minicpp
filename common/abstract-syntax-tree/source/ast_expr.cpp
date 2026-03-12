#include "../ast_expr.hpp"

ASTExpr::ASTExpr(const Token& token, ASTNodeType ntype, Type type) : ASTNode(token, ntype), type{ type } {}

Type ASTExpr::getType() const noexcept {
    return type;
}

void ASTExpr::setType(Type exprType) noexcept {
    type = exprType;
}