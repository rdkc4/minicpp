#include "../ast_return_stmt.hpp"

#include "../defs/ast_defs.hpp"

ASTReturnStmt::ASTReturnStmt(const Token& token) : ASTStmt(token, ASTNodeType::RETURN_STMT) {}

void ASTReturnStmt::setReturnExpr(std::unique_ptr<ASTExpr> expr){
    returnExpr = std::move(expr);
}

bool ASTReturnStmt::hasReturnExpr() const noexcept {
    return returnExpr != nullptr;
}

void ASTReturnStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}