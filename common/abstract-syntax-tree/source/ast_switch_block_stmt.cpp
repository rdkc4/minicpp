#include "../ast_switch_block_stmt.hpp"

ASTSwitchBlockStmt::ASTSwitchBlockStmt(const Token& token, ASTNodeType ntype) : ASTStmt(token, ntype) {}

const std::vector<std::unique_ptr<ASTStmt>>& ASTSwitchBlockStmt::getStatements() const noexcept {
    return statements;
}

void ASTSwitchBlockStmt::addStatement(std::unique_ptr<ASTStmt> statement){
    statements.push_back(std::move(statement));
}

void ASTSwitchBlockStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}