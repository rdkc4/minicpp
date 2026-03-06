#include "../ASTInclude.hpp"

ASTInclude::ASTInclude(const Token token, ASTNodeType ntype) : ASTDirective(token, ntype) {}

const std::string& ASTInclude::getLibName() const noexcept {
    return libName;
}

void ASTInclude::setLibName(const std::string _libName) {
    libName = _libName;
}