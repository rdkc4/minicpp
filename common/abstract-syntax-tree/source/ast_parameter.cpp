#include "../ast_parameter.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTParameter::ASTParameter(const Token& token, Type type) 
    : ASTNode(token, AST::defs::ASTNodeType::PARAMETER), type{ type } {}

Type AST::node::ASTParameter::getType() const noexcept {
    return type;
}

void AST::node::ASTParameter::setType(Type parameterType) noexcept {
    type = parameterType;
}

void AST::node::ASTParameter::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}