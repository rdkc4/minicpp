#include "../ir_binary_expr.hpp"

#include "../defs/ir_defs.hpp"

ir::IRBinaryExpr::IRBinaryExpr(ir::IRNodeType ntype, types::Type type) 
    : IRExpr(ntype, type) {}

syntax::Operator ir::IRBinaryExpr::getOperator() const noexcept {
    return exprOperator;
}

void ir::IRBinaryExpr::setBinaryExpr(
    std::unique_ptr<IRExpr> leftOperand, 
    std::unique_ptr<IRExpr> rightOperand, 
    syntax::Operator op
){
    leftOperandExpr = std::move(leftOperand);
    rightOperandExpr = std::move(rightOperand);
    exprOperator = op;
}

void ir::IRBinaryExpr::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}