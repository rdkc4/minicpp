#include "../ir_expr.hpp"

IR::node::IRExpr::IRExpr(IR::defs::IRNodeType ntype, Type type) 
    : IRNode(ntype), type{ type } {}

Type IR::node::IRExpr::getType() const noexcept {
    return type;
}

void IR::node::IRExpr::setType(Type exprType) noexcept {
    type = exprType;
}