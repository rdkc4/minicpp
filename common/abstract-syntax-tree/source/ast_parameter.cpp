#include "../ast_parameter.hpp"

ASTParameter::ASTParameter(const Token& token, ASTNodeType ntype, Type type) : ASTNode(token, ntype), type{ type } {}

Type ASTParameter::getType() const noexcept {
    return type;
}

void ASTParameter::setType(Type t) noexcept {
    type = t;
}

void ASTParameter::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}