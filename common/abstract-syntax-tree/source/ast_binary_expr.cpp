#include "../ast_binary_expr.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTBinaryExpr::ASTBinaryExpr(const syntax::Token& token, Type type, Operator op) 
    : ASTExpr(token, syntax::ast::ASTNodeType::BINARY_EXPR, type), exprOperator{ op } {}

Operator syntax::ast::ASTBinaryExpr::getOperator() const noexcept {
    return exprOperator;
}

void syntax::ast::ASTBinaryExpr::setOperator(Operator op) noexcept {
    exprOperator = op;
}

void syntax::ast::ASTBinaryExpr::setOperandExprs(
    std::unique_ptr<syntax::ast::ASTExpr> leftOperand, 
    std::unique_ptr<syntax::ast::ASTExpr> rightOperand
){
    leftOperandExpr = std::move(leftOperand);
    rightOperandExpr = std::move(rightOperand);
}

bool syntax::ast::ASTBinaryExpr::initialized() const noexcept {
    return leftOperandExpr != nullptr && rightOperandExpr != nullptr;
}

void syntax::ast::ASTBinaryExpr::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}