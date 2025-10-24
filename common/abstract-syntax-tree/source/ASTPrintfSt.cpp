#include "../ASTPrintfSt.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <format>

ASTPrintfSt::ASTPrintfSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

void ASTPrintfSt::setExp(std::unique_ptr<ASTExpression> nexp){
    exp = std::move(nexp);
}

void ASTPrintfSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    exp->print(offset + 1);
}