#include "../ASTCompoundSt.hpp"

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <format>

ASTCompoundSt::ASTCompoundSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

const std::vector<std::unique_ptr<ASTStatement>>& ASTCompoundSt::getStatements() const noexcept {
    return statements;
}

void ASTCompoundSt::addStatement(std::unique_ptr<ASTStatement> statement){
    statements.push_back(std::move(statement));
}

void ASTCompoundSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    for(const auto& statement : statements){
        statement->print(offset + 1);
    }
}