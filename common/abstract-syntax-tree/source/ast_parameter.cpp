#include "../ast_parameter.hpp"

#include "../defs/ast_defs.hpp"

ASTParameter::ASTParameter(const Token& token, Type type) 
    : ASTNode(token, ASTNodeType::PARAMETER), type{ type } {}

Type ASTParameter::getType() const noexcept {
    return type;
}

void ASTParameter::setType(Type parameterType) noexcept {
    type = parameterType;
}

void ASTParameter::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}