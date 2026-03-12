#include "../ir_expr.hpp"

IRExpr::IRExpr(IRNodeType ntype, Type type) : IRNode(ntype), type{ type } {}

Type IRExpr::getType() const noexcept {
    return type;
}

void IRExpr::setType(Type exprType) noexcept {
    type = exprType;
}