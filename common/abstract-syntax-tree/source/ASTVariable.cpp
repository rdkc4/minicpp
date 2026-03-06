#include "../ASTVariable.hpp"

ASTVariable::ASTVariable(const Token& token, ASTNodeType ntype, Types type) : ASTStatement(token, ntype), type{ type } {}

Types ASTVariable::getType() const noexcept {
    return type;
}

void ASTVariable::setType(Types t) noexcept {
    type = t;
}

void ASTVariable::setAssign(std::unique_ptr<ASTExpression> assign){
    assignment = std::move(assign);
}

bool ASTVariable::hasAssign() const noexcept {
    return assignment != nullptr;
}