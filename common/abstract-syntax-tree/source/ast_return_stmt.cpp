#include "../ast_return_stmt.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTReturnStmt::ASTReturnStmt(const Token& token) 
    : ASTStmt(token, AST::defs::ASTNodeType::RETURN_STMT) {}

void AST::node::ASTReturnStmt::setReturnExpr(std::unique_ptr<AST::node::ASTExpr> expr){
    returnExpr = std::move(expr);
}

bool AST::node::ASTReturnStmt::hasReturnExpr() const noexcept {
    return returnExpr != nullptr;
}

void AST::node::ASTReturnStmt::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}