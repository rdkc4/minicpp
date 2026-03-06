#include "../ast_variable_decl_stmt.hpp"

ASTVariableDeclStmt::ASTVariableDeclStmt(const Token& token, ASTNodeType ntype, Type type) : ASTStmt(token, ntype), type{ type } {}

Type ASTVariableDeclStmt::getType() const noexcept {
    return type;
}

void ASTVariableDeclStmt::setType(Type t) noexcept {
    type = t;
}

void ASTVariableDeclStmt::setAssign(std::unique_ptr<ASTExpr> assign){
    assignment = std::move(assign);
}

bool ASTVariableDeclStmt::hasAssign() const noexcept {
    return assignment != nullptr;
}

void ASTVariableDeclStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}