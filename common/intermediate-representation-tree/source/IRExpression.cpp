#include "../IRExpression.hpp"

IRExpression::IRExpression(IRNodeType ntype, Types type) : IRNode(ntype), type{ type } {}

Types IRExpression::getType() const noexcept {
    return type;
}

void IRExpression::setType(Types t) noexcept {
    type = t;
}