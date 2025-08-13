#include "../IRForSt.hpp"

#include <memory>
#include <iostream>
#include <string>
#include <format>

IRForSt::IRForSt(IRNodeType ntype) : IRStatement(ntype) {}

const IRAssignSt* IRForSt::getInitializer() const noexcept {
    return initializer.get();
}

const IRExpression* IRForSt::getCondition() const noexcept {
    return condition.get();
}

const IRAssignSt* IRForSt::getIncrementer() const noexcept {
    return incrementer.get();
}

const IRStatement* IRForSt::getStatement() const noexcept {
    return statement.get();
}

void IRForSt::setForSt(std::unique_ptr<IRAssignSt> init, std::unique_ptr<IRExpression> cond, std::unique_ptr<IRAssignSt> inc, std::unique_ptr<IRStatement> st, std::unique_ptr<IRTemporary> temp){
    initializer = std::move(init);
    condition = std::move(cond);
    incrementer = std::move(inc);
    statement = std::move(st);
    temporaries = std::move(temp);
}

bool IRForSt::hasInitializer() const noexcept {
    return initializer != nullptr;
}

bool IRForSt::hasCondition() const noexcept {
    return condition != nullptr;
}

bool IRForSt::hasIncrementer() const noexcept {
    return incrementer != nullptr;
}

const IRTemporary* IRForSt::getTemporaries() const noexcept {
    return temporaries.get();
}

bool IRForSt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}

void IRForSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    if(temporaries != nullptr){
        temporaries->print(offset + 1);
    }
    if(hasInitializer()){
        initializer->print(offset + 1);
    }
    if(hasCondition()){
        condition->print(offset + 1);
    }
    if(hasIncrementer()){
        incrementer->print(offset + 1);
    }
    statement->print(offset + 1);
}