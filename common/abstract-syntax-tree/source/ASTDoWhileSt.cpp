#include "../ASTDoWhileSt.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <format>

ASTDoWhileSt::ASTDoWhileSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

const ASTStatement* ASTDoWhileSt::getStatement() const noexcept {
    return statement.get();
}

void ASTDoWhileSt::setDoWhile(std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTStatement> st){
    condition = std::move(cond);
    statement = std::move(st);
}

void ASTDoWhileSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    condition->print(offset + 1);
    statement->print(offset + 1);
}