#include "../ASTReturnSt.hpp"

ASTReturnSt::ASTReturnSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

void ASTReturnSt::setExp(std::unique_ptr<ASTExpression> nexp){
    exp = std::move(nexp);
}

bool ASTReturnSt::returns() const noexcept {
    return exp != nullptr;
}