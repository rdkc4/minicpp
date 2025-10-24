#include "../ASTWhileSt.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <format>

ASTWhileSt::ASTWhileSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

const ASTStatement* ASTWhileSt::getStatement() const noexcept {
    return statement.get();
}

void ASTWhileSt::setDoWhile(std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTStatement> st){
    condition = std::move(cond);
    statement = std::move(st);
}

void ASTWhileSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    condition->print(offset + 1);
    statement->print(offset + 1);
}