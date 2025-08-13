#include "../IRWhileSt.hpp"

#include <memory>
#include <iostream>
#include <format>
#include <string>

IRWhileSt::IRWhileSt(IRNodeType ntype) : IRStatement(ntype) {}

const IRExpression* IRWhileSt::getCondition() const noexcept {
    return condition.get();
}

const IRStatement* IRWhileSt::getStatement() const noexcept {
    return statement.get();
}

void IRWhileSt::setWhileSt(std::unique_ptr<IRExpression> cond, std::unique_ptr<IRStatement> st, std::unique_ptr<IRTemporary> temp){
    condition = std::move(cond);
    statement = std::move(st);
    temporaries = std::move(temp);
}

const IRTemporary* IRWhileSt::getTemporaries() const noexcept {
    return temporaries.get();
}

bool IRWhileSt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}

void IRWhileSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    if(temporaries != nullptr){
        temporaries->print(offset + 1);
    }
    condition->print(offset + 1);
    statement->print(offset + 1);
}