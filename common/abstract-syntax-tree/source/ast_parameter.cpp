#include "../ast_parameter.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTParameter::ASTParameter(const syntax::Token& token, types::Type type) 
    : ASTNode(token, syntax::ast::ASTNodeType::PARAMETER), type{ type } {}

types::Type syntax::ast::ASTParameter::getType() const noexcept {
    return type;
}

void syntax::ast::ASTParameter::setType(types::Type parameterType) noexcept {
    type = parameterType;
}

void syntax::ast::ASTParameter::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}