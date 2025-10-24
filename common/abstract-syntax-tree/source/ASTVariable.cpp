#include "../ASTVariable.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <format>

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

void ASTVariable::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    if(assignment != nullptr){
        assignment->print(offset + 1);
    }
}