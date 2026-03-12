#include "../ir_binary_expr.hpp"

#include "../defs/ir_defs.hpp"

IRBinaryExpr::IRBinaryExpr(IRNodeType ntype, Type type) : IRExpr(ntype, type) {}

Operator IRBinaryExpr::getOperator() const noexcept {
    return exprOperator;
}

void IRBinaryExpr::setBinaryExpr(std::unique_ptr<IRExpr> leftOperand, std::unique_ptr<IRExpr> rightOperand, Operator op){
    leftOperandExpr = std::move(leftOperand);
    rightOperandExpr = std::move(rightOperand);
    exprOperator = op;
}

void IRBinaryExpr::accept(IRVisitor& visitor){
    visitor.visit(this);
}