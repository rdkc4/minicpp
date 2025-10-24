#include "../ASTBinaryExpression.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <format>

ASTBinaryExpression::ASTBinaryExpression(const Token& token, ASTNodeType ntype, Types type, Operators op) : ASTExpression(token, ntype, type), op{ op } {}

Operators ASTBinaryExpression::getOperator() const noexcept {
    return op;
}

void ASTBinaryExpression::setOperator(Operators _op) noexcept {
    op = _op;
}

void ASTBinaryExpression::setOperands(std::unique_ptr<ASTExpression> lOp, std::unique_ptr<ASTExpression> rOp){
    leftOperand = std::move(lOp);
    rightOperand = std::move(rOp);
}

bool ASTBinaryExpression::initialized() const noexcept {
    return leftOperand != nullptr && rightOperand != nullptr;
}

void ASTBinaryExpression::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    leftOperand->print(offset + 1);
    rightOperand->print(offset + 1);
}