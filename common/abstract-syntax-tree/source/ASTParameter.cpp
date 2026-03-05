#include "../ASTParameter.hpp"

ASTParameter::ASTParameter(const Token& token, ASTNodeType ntype, Types type) : ASTNode(token, ntype), type{ type } {}

Types ASTParameter::getType() const noexcept {
    return type;
}

void ASTParameter::setType(Types t) noexcept {
    type = t;
}