#include "../ast_include_dir.hpp"

#include "../defs/ast_defs.hpp"

ASTIncludeDir::ASTIncludeDir(const Token token) : ASTDir(token, ASTNodeType::INCLUDE_DIR) {}

const std::string& ASTIncludeDir::getLibName() const noexcept {
    return libName;
}

void ASTIncludeDir::setLibName(const std::string& includedLibName) {
    libName = includedLibName;
}

void ASTIncludeDir::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}