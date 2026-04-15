#include "../ast_variable_decl_stmt.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTVariableDeclStmt::ASTVariableDeclStmt(const Token& token, Type type) 
    : ASTStmt(token, AST::defs::ASTNodeType::VARIABLE_DECL_STMT), type{ type } {}

Type AST::node::ASTVariableDeclStmt::getType() const noexcept {
    return type;
}

void AST::node::ASTVariableDeclStmt::setType(Type variableType) noexcept {
    type = variableType;
}

void AST::node::ASTVariableDeclStmt::setAssignExpr(std::unique_ptr<AST::node::ASTExpr> expr){
    assignExpr = std::move(expr);
}

bool AST::node::ASTVariableDeclStmt::hasAssignExpr() const noexcept {
    return assignExpr != nullptr;
}

void AST::node::ASTVariableDeclStmt::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}