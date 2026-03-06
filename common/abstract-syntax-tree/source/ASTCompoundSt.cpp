#include "../ASTCompoundSt.hpp"

ASTCompoundSt::ASTCompoundSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

const std::vector<std::unique_ptr<ASTStatement>>& ASTCompoundSt::getStatements() const noexcept {
    return statements;
}

void ASTCompoundSt::addStatement(std::unique_ptr<ASTStatement> statement){
    statements.push_back(std::move(statement));
}