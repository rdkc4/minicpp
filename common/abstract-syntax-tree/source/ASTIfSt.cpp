#include "../ASTIfSt.hpp"

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <format>

ASTIfSt::ASTIfSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

const std::vector<std::unique_ptr<ASTExpression>>& ASTIfSt::getConditions() const noexcept {
    return conditions;
}
const std::vector<std::unique_ptr<ASTStatement>>& ASTIfSt::getStatements() const noexcept {
    return statements;
}

void ASTIfSt::addIf(std::unique_ptr<ASTExpression> condition, std::unique_ptr<ASTStatement> statement){
    conditions.push_back(std::move(condition));
    statements.push_back(std::move(statement));
}

void ASTIfSt::addElse(std::unique_ptr<ASTStatement> statement){
    statements.push_back(std::move(statement));
}

bool ASTIfSt::hasElse() const noexcept {
    return statements.size() > conditions.size();
}

void ASTIfSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    for(const auto& condition : conditions){
        condition->print(offset + 1);
    }
    for(const auto& statement : statements){
        statement->print(offset + 1);
    }
}