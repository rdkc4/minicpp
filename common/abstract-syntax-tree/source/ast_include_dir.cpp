#include "../ast_include_dir.hpp"

ASTIncludeDir::ASTIncludeDir(const Token token, ASTNodeType ntype) : ASTDir(token, ntype) {}

const std::string& ASTIncludeDir::getLibName() const noexcept {
    return libName;
}

void ASTIncludeDir::setLibName(const std::string _libName) {
    libName = _libName;
}

void ASTIncludeDir::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}