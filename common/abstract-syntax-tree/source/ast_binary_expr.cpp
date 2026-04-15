#include "../ast_binary_expr.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTBinaryExpr::ASTBinaryExpr(const Token& token, Type type, Operator op) 
    : ASTExpr(token, AST::defs::ASTNodeType::BINARY_EXPR, type), exprOperator{ op } {}

Operator AST::node::ASTBinaryExpr::getOperator() const noexcept {
    return exprOperator;
}

void AST::node::ASTBinaryExpr::setOperator(Operator op) noexcept {
    exprOperator = op;
}

void AST::node::ASTBinaryExpr::setOperandExprs(std::unique_ptr<AST::node::ASTExpr> leftOperand, std::unique_ptr<AST::node::ASTExpr> rightOperand){
    leftOperandExpr = std::move(leftOperand);
    rightOperandExpr = std::move(rightOperand);
}

bool AST::node::ASTBinaryExpr::initialized() const noexcept {
    return leftOperandExpr != nullptr && rightOperandExpr != nullptr;
}

void AST::node::ASTBinaryExpr::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}