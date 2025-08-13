#include "../IRReturnSt.hpp"

#include <memory>
#include <iostream>
#include <format>
#include <string>

IRReturnSt::IRReturnSt(IRNodeType ntype) : IRStatement(ntype) {}

const IRExpression* IRReturnSt::getExp() const noexcept {
    return exp.get();
}

void IRReturnSt::setExp(std::unique_ptr<IRExpression> _exp, std::unique_ptr<IRTemporary> temp){
    exp = std::move(_exp);
    temporaries = std::move(temp);
}

const IRTemporary* IRReturnSt::getTemporaries() const noexcept {
    return temporaries.get();
}

bool IRReturnSt::returns() const noexcept {
    return exp != nullptr;
}

bool IRReturnSt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}

void IRReturnSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    if(temporaries != nullptr){
        temporaries->print(offset + 1);
    }
    exp->print(offset + 1);
}