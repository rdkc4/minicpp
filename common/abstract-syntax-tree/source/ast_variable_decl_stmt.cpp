#include "../ast_variable_decl_stmt.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTVariableDeclStmt::ASTVariableDeclStmt(const syntax::Token& token, Type type) 
    : ASTStmt(token, syntax::ast::ASTNodeType::VARIABLE_DECL_STMT), type{ type } {}

Type syntax::ast::ASTVariableDeclStmt::getType() const noexcept {
    return type;
}

void syntax::ast::ASTVariableDeclStmt::setType(Type variableType) noexcept {
    type = variableType;
}

void syntax::ast::ASTVariableDeclStmt::setAssignExpr(std::unique_ptr<syntax::ast::ASTExpr> expr){
    assignExpr = std::move(expr);
}

bool syntax::ast::ASTVariableDeclStmt::hasAssignExpr() const noexcept {
    return assignExpr != nullptr;
}

void syntax::ast::ASTVariableDeclStmt::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}