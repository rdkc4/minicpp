#include "../ast_binary_expr.hpp"

ASTBinaryExpr::ASTBinaryExpr(const Token& token, ASTNodeType ntype, Type type, Operator op) : ASTExpr(token, ntype, type), op{ op } {}

Operator ASTBinaryExpr::getOperator() const noexcept {
    return op;
}

void ASTBinaryExpr::setOperator(Operator _op) noexcept {
    op = _op;
}

void ASTBinaryExpr::setOperands(std::unique_ptr<ASTExpr> lOp, std::unique_ptr<ASTExpr> rOp){
    leftOperand = std::move(lOp);
    rightOperand = std::move(rOp);
}

bool ASTBinaryExpr::initialized() const noexcept {
    return leftOperand != nullptr && rightOperand != nullptr;
}

void ASTBinaryExpr::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}