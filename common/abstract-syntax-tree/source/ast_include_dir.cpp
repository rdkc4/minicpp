#include "../ast_include_dir.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTIncludeDir::ASTIncludeDir(const syntax::Token& token) 
    : ASTDir(token, syntax::ast::ASTNodeType::INCLUDE_DIR) {}

const std::string& syntax::ast::ASTIncludeDir::getLibName() const noexcept {
    return libName;
}

void syntax::ast::ASTIncludeDir::setLibName(const std::string& includedLibName) {
    libName = includedLibName;
}

void syntax::ast::ASTIncludeDir::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}