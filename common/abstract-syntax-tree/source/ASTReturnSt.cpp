#include "../ASTReturnSt.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <format>

ASTReturnSt::ASTReturnSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

void ASTReturnSt::setExp(std::unique_ptr<ASTExpression> nexp){
    exp = std::move(nexp);
}

bool ASTReturnSt::returns() const noexcept {
    return exp != nullptr;
}

void ASTReturnSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    exp->print(offset + 1);
}