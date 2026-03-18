#include "../ast_default_stmt.hpp"

#include "../defs/ast_defs.hpp"

ASTDefaultStmt::ASTDefaultStmt(const Token& token) : ASTStmt(token, ASTNodeType::DEFAULT_STMT) {}

void ASTDefaultStmt::setDefaultStmt(std::unique_ptr<ASTSwitchBlockStmt> swBlockStmt){
    switchBlockStmt = std::move(swBlockStmt);
}

void ASTDefaultStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}