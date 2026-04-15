#include "../ast_expr.hpp"

AST::node::ASTExpr::ASTExpr(const Token& token, AST::defs::ASTNodeType ntype, Type type) 
    : ASTNode(token, ntype), type{ type } {}

Type AST::node::ASTExpr::getType() const noexcept {
    return type;
}

void AST::node::ASTExpr::setType(Type exprType) noexcept {
    type = exprType;
}