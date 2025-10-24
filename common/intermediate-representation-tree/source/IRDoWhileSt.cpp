#include "../IRDoWhileSt.hpp"

#include <memory>
#include <format>
#include <string>
#include <iostream>

IRDoWhileSt::IRDoWhileSt(IRNodeType ntype) : IRStatement(ntype) {}

const IRExpression* IRDoWhileSt::getCondition() const noexcept {
    return condition.get();
}

void IRDoWhileSt::setDoWhileSt(std::unique_ptr<IRExpression> cond, std::unique_ptr<IRStatement> st, std::unique_ptr<IRTemporary> temp){
    condition = std::move(cond);
    statement = std::move(st);
    temporaries = std::move(temp);
}

const IRTemporary* IRDoWhileSt::getTemporaries() const noexcept {
    return temporaries.get();
}

bool IRDoWhileSt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}

void IRDoWhileSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    if(temporaries != nullptr){
        temporaries->print(offset + 1);
    }
    condition->print(offset + 1);
    statement->print(offset + 1);
}