#include "../ASTSwitchBlock.hpp"

ASTSwitchBlock::ASTSwitchBlock(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

const std::vector<std::unique_ptr<ASTStatement>>& ASTSwitchBlock::getStatements() const noexcept {
    return statements;
}

void ASTSwitchBlock::addStatement(std::unique_ptr<ASTStatement> statement){
    statements.push_back(std::move(statement));
}