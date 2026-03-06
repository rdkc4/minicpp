#include "../ASTDoWhileSt.hpp"

ASTDoWhileSt::ASTDoWhileSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

const ASTStatement* ASTDoWhileSt::getStatement() const noexcept {
    return statement.get();
}

void ASTDoWhileSt::setDoWhile(std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTStatement> st){
    condition = std::move(cond);
    statement = std::move(st);
}