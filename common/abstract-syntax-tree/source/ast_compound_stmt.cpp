#include "../ast_compound_stmt.hpp"

#include "../defs/ast_defs.hpp"

ASTCompoundStmt::ASTCompoundStmt(const Token& token) : ASTStmt(token, ASTNodeType::COMPOUND_STATEMENT) {}

const std::vector<std::unique_ptr<ASTStmt>>& ASTCompoundStmt::getStmts() const noexcept {
    return stmts;
}

void ASTCompoundStmt::addStmt(std::unique_ptr<ASTStmt> stmt){
    stmts.push_back(std::move(stmt));
}

void ASTCompoundStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}