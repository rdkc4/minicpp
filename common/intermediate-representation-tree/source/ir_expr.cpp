#include "../ir_expr.hpp"

ir::IRExpr::IRExpr(ir::IRNodeType ntype, types::Type type) 
    : IRNode(ntype), type{ type } {}

types::Type ir::IRExpr::getType() const noexcept {
    return type;
}

void ir::IRExpr::setType(types::Type exprType) noexcept {
    type = exprType;
}