#include "../ir_binary_expr.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRBinaryExpr::IRBinaryExpr(IR::defs::IRNodeType ntype, Type type) : IRExpr(ntype, type) {}

Operator IR::node::IRBinaryExpr::getOperator() const noexcept {
    return exprOperator;
}

void IR::node::IRBinaryExpr::setBinaryExpr(std::unique_ptr<IRExpr> leftOperand, std::unique_ptr<IRExpr> rightOperand, Operator op){
    leftOperandExpr = std::move(leftOperand);
    rightOperandExpr = std::move(rightOperand);
    exprOperator = op;
}

void IR::node::IRBinaryExpr::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}