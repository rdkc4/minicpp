#include "../ASTExpression.hpp"

ASTExpression::ASTExpression(const Token& token, ASTNodeType ntype, Types type) : ASTNode(token, ntype), type{ type } {}

Types ASTExpression::getType() const noexcept {
    return type;
}

void ASTExpression::setType(Types t) noexcept {
    type = t;
}