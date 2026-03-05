#include "../ASTWhileSt.hpp"

ASTWhileSt::ASTWhileSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

const ASTStatement* ASTWhileSt::getStatement() const noexcept {
    return statement.get();
}

void ASTWhileSt::setWhile(std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTStatement> st){
    condition = std::move(cond);
    statement = std::move(st);
}