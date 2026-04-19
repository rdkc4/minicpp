#include "../ast_expr.hpp"

syntax::ast::ASTExpr::ASTExpr(const syntax::Token& token, syntax::ast::ASTNodeType ntype, types::Type type) 
    : ASTNode(token, ntype), type{ type } {}

types::Type syntax::ast::ASTExpr::getType() const noexcept {
    return type;
}

void syntax::ast::ASTExpr::setType(types::Type exprType) noexcept {
    type = exprType;
}