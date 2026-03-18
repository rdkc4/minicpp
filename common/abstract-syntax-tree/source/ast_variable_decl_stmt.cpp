#include "../ast_variable_decl_stmt.hpp"

#include "../defs/ast_defs.hpp"

ASTVariableDeclStmt::ASTVariableDeclStmt(const Token& token, Type type) 
    : ASTStmt(token, ASTNodeType::VARIABLE_DECL_STMT), type{ type } {}

Type ASTVariableDeclStmt::getType() const noexcept {
    return type;
}

void ASTVariableDeclStmt::setType(Type variableType) noexcept {
    type = variableType;
}

void ASTVariableDeclStmt::setAssignExpr(std::unique_ptr<ASTExpr> expr){
    assignExpr = std::move(expr);
}

bool ASTVariableDeclStmt::hasAssignExpr() const noexcept {
    return assignExpr != nullptr;
}

void ASTVariableDeclStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}