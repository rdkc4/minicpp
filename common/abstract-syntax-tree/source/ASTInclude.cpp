#include "../ASTInclude.hpp"

#include <iostream>
#include "../ASTDirective.hpp"

ASTInclude::ASTInclude(const Token token, ASTNodeType ntype) : ASTDirective(token, ntype) {}

const std::string& ASTInclude::getLibName() const noexcept {
    return libName;
}

void ASTInclude::setLibName(const std::string _libName) {
    libName = _libName;
}

void ASTInclude::print(size_t offset) const {
    std::cout << std::format("{}|-> {} {}", std::string(offset * 2, ' '), libName, toString());
}