#include "../ast_switch_block_stmt.hpp"

#include "../defs/ast_defs.hpp"

ASTSwitchBlockStmt::ASTSwitchBlockStmt(const Token& token) : ASTStmt(token, ASTNodeType::SWITCH_BLOCK_STMT) {}

const std::vector<std::unique_ptr<ASTStmt>>& ASTSwitchBlockStmt::getStmts() const noexcept {
    return stmts;
}

void ASTSwitchBlockStmt::addStmt(std::unique_ptr<ASTStmt> stmt){
    stmts.push_back(std::move(stmt));
}

void ASTSwitchBlockStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}