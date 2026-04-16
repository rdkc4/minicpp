#include "../ast_include_dir.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTIncludeDir::ASTIncludeDir(const Token token) 
    : ASTDir(token, AST::defs::ASTNodeType::INCLUDE_DIR) {}

const std::string& AST::node::ASTIncludeDir::getLibName() const noexcept {
    return libName;
}

void AST::node::ASTIncludeDir::setLibName(const std::string& includedLibName) {
    libName = includedLibName;
}

void AST::node::ASTIncludeDir::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}