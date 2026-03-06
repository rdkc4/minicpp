#include "../ast_compound_stmt.hpp"

ASTCompoundStmt::ASTCompoundStmt(const Token& token, ASTNodeType ntype) : ASTStmt(token, ntype) {}

const std::vector<std::unique_ptr<ASTStmt>>& ASTCompoundStmt::getStatements() const noexcept {
    return statements;
}

void ASTCompoundStmt::addStatement(std::unique_ptr<ASTStmt> statement){
    statements.push_back(std::move(statement));
}

void ASTCompoundStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}