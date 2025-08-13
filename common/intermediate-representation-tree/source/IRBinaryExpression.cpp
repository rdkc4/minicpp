#include "../IRBinaryExpression.hpp"

#include <memory>
#include <iostream>
#include <string>
#include <format>

IRBinaryExpression::IRBinaryExpression(IRNodeType ntype, Types type) : IRExpression(ntype, type) {}

const IRExpression* IRBinaryExpression::getLeftOperand() const noexcept {
    return leftOperand.get();
}

const IRExpression* IRBinaryExpression::getRightOperand() const noexcept {
    return rightOperand.get();
}

Operators IRBinaryExpression::getOperator() const noexcept {
    return op;
}

void IRBinaryExpression::setBinaryExpression(std::unique_ptr<IRExpression> lOp, std::unique_ptr<IRExpression> rOp, Operators _op){
    leftOperand = std::move(lOp);
    rightOperand = std::move(rOp);
    op = _op;
}

void IRBinaryExpression::print(size_t offset) const {
    std::cout << std::format("{}|-> {} | {}", std::string(offset*2, ' '), operatorToString.at(op), toString());
    leftOperand->print(offset + 1);
    rightOperand->print(offset + 1);
}