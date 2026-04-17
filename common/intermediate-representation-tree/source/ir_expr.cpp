#include "../ir_expr.hpp"

ir::IRExpr::IRExpr(ir::IRNodeType ntype, Type type) 
    : IRNode(ntype), type{ type } {}

Type ir::IRExpr::getType() const noexcept {
    return type;
}

void ir::IRExpr::setType(Type exprType) noexcept {
    type = exprType;
}