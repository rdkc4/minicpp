#include "../ast_return_stmt.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTReturnStmt::ASTReturnStmt(const syntax::Token& token) 
    : ASTStmt(token, syntax::ast::ASTNodeType::RETURN_STMT) {}

void syntax::ast::ASTReturnStmt::setReturnExpr(std::unique_ptr<syntax::ast::ASTExpr> expr){
    returnExpr = std::move(expr);
}

bool syntax::ast::ASTReturnStmt::hasReturnExpr() const noexcept {
    return returnExpr != nullptr;
}

void syntax::ast::ASTReturnStmt::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}