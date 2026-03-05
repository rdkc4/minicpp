#include "../ASTBinaryExpression.hpp"

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