#include "../ast_default_stmt.hpp"

ASTDefaultStmt::ASTDefaultStmt(const Token& token, ASTNodeType ntype) : ASTStmt(token, ntype) {}

void ASTDefaultStmt::setDefault(std::unique_ptr<ASTSwitchBlockStmt> _swBlock){
    swBlock = std::move(_swBlock);
}

void ASTDefaultStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}