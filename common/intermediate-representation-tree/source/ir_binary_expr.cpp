#include "../ir_binary_expr.hpp"

IRBinaryExpr::IRBinaryExpr(IRNodeType ntype, Type type) : IRExpr(ntype, type) {}

const IRExpr* IRBinaryExpr::getLeftOperand() const noexcept {
    return leftOperand.get();
}

const IRExpr* IRBinaryExpr::getRightOperand() const noexcept {
    return rightOperand.get();
}

Operator IRBinaryExpr::getOperator() const noexcept {
    return op;
}

void IRBinaryExpr::setBinaryExpression(std::unique_ptr<IRExpr> lOp, std::unique_ptr<IRExpr> rOp, Operator _op){
    leftOperand = std::move(lOp);
    rightOperand = std::move(rOp);
    op = _op;
}