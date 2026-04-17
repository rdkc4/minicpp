#include "../ir_binary_expr.hpp"

#include "../defs/ir_defs.hpp"

ir::IRBinaryExpr::IRBinaryExpr(ir::IRNodeType ntype, Type type) 
    : IRExpr(ntype, type) {}

Operator ir::IRBinaryExpr::getOperator() const noexcept {
    return exprOperator;
}

void ir::IRBinaryExpr::setBinaryExpr(
    std::unique_ptr<IRExpr> leftOperand, 
    std::unique_ptr<IRExpr> rightOperand, 
    Operator op
){
    leftOperandExpr = std::move(leftOperand);
    rightOperandExpr = std::move(rightOperand);
    exprOperator = op;
}

void ir::IRBinaryExpr::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}