#include "../ASTParameter.hpp"

#include <string>
#include <iostream>
#include <format>

ASTParameter::ASTParameter(const Token& token, ASTNodeType ntype, Types type) : ASTNode(token, ntype), type{ type } {}

Types ASTParameter::getType() const noexcept {
    return type;
}

void ASTParameter::setType(Types t) noexcept {
    type = t;
}

void ASTParameter::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
}