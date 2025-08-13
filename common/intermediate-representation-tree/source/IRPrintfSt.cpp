#include "../IRPrintfSt.hpp"

#include <memory>
#include <iostream>
#include <format>
#include <string>

IRPrintfSt::IRPrintfSt(IRNodeType ntype) : IRStatement(ntype) {}

const IRExpression* IRPrintfSt::getExp() const noexcept {
    return exp.get();
}

void IRPrintfSt::setExp(std::unique_ptr<IRExpression> _exp, std::unique_ptr<IRTemporary> temp){
    exp = std::move(_exp);
    temporaries = std::move(temp);
}

const IRTemporary* IRPrintfSt::getTemporaries() const noexcept {
    return temporaries.get();
}

void IRPrintfSt::setTemporaries(std::unique_ptr<IRTemporary> temp){
    temporaries = std::move(temp);
}

bool IRPrintfSt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}

void IRPrintfSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    if(temporaries != nullptr){
        temporaries->print(offset + 1);
    }
    exp->print(offset + 1);
}