#include "../ast_binary_expr.hpp"

#include "../defs/ast_defs.hpp"

ASTBinaryExpr::ASTBinaryExpr(const Token& token, Type type, Operator op) : ASTExpr(token, ASTNodeType::BINARY_EXPRESSION, type), expressionOperator{ op } {}

Operator ASTBinaryExpr::getOperator() const noexcept {
    return expressionOperator;
}

void ASTBinaryExpr::setOperator(Operator op) noexcept {
    expressionOperator = op;
}

void ASTBinaryExpr::setOperandExprs(std::unique_ptr<ASTExpr> leftOperand, std::unique_ptr<ASTExpr> rightOperand){
    leftOperandExpr = std::move(leftOperand);
    rightOperandExpr = std::move(rightOperand);
}

bool ASTBinaryExpr::initialized() const noexcept {
    return leftOperandExpr != nullptr && rightOperandExpr != nullptr;
}

void ASTBinaryExpr::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}