#include "../ir_binary_expr.hpp"

IRBinaryExpr::IRBinaryExpr(IRNodeType ntype, Type type) : IRExpr(ntype, type) {}

Operator IRBinaryExpr::getOperator() const noexcept {
    return op;
}

void IRBinaryExpr::setBinaryExpression(std::unique_ptr<IRExpr> lOp, std::unique_ptr<IRExpr> rOp, Operator _op){
    leftOperand = std::move(lOp);
    rightOperand = std::move(rOp);
    op = _op;
}

void IRBinaryExpr::accept(IRVisitor& visitor){
    visitor.visit(this);
}