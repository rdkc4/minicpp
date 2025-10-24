#include "../ASTAssignSt.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <format>

ASTAssignSt::ASTAssignSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

void ASTAssignSt::setVariable(std::unique_ptr<ASTId> var){
    variable = std::move(var);
}

void ASTAssignSt::setExp(std::unique_ptr<ASTExpression> nexp){
    exp = std::move(nexp);
}

void ASTAssignSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    variable->print(offset + 1);
    exp->print(offset + 1);
}