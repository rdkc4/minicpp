#include "../ast_return_stmt.hpp"

ASTReturnStmt::ASTReturnStmt(const Token& token, ASTNodeType ntype) : ASTStmt(token, ntype) {}

void ASTReturnStmt::setExp(std::unique_ptr<ASTExpr> nexp){
    exp = std::move(nexp);
}

bool ASTReturnStmt::returns() const noexcept {
    return exp != nullptr;
}

void ASTReturnStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}