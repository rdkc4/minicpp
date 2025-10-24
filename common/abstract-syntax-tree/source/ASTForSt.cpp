#include "../ASTForSt.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <format>

ASTForSt::ASTForSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

const ASTAssignSt* ASTForSt::getInitializer() const noexcept {
    return initializer.get();
}

const ASTAssignSt* ASTForSt::getIncrementer() const noexcept{
    return incrementer.get();
}

const ASTStatement* ASTForSt::getStatement() const noexcept {
    return statement.get();
}

void ASTForSt::setForSt(std::unique_ptr<ASTAssignSt> init, std::unique_ptr<ASTExpression> cond, std::unique_ptr<ASTAssignSt> inc, std::unique_ptr<ASTStatement> st){
    initializer = std::move(init);
    condition = std::move(cond);
    incrementer = std::move(inc);
    statement = std::move(st);
}

bool ASTForSt::hasInitializer() const noexcept {
    return initializer != nullptr;
}

bool ASTForSt::hasCondition() const noexcept {
    return condition != nullptr;
}

bool ASTForSt::hasIncrementer() const noexcept {
    return incrementer != nullptr;
}

void ASTForSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    initializer->print(offset + 1);
    condition->print(offset + 1);
    incrementer->print(offset + 1);
    statement->print(offset + 1);
}