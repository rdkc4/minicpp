#include "../ast_expr.hpp"

syntax::ast::ASTExpr::ASTExpr(const syntax::Token& token, syntax::ast::ASTNodeType ntype, Type type) 
    : ASTNode(token, ntype), type{ type } {}

Type syntax::ast::ASTExpr::getType() const noexcept {
    return type;
}

void syntax::ast::ASTExpr::setType(Type exprType) noexcept {
    type = exprType;
}