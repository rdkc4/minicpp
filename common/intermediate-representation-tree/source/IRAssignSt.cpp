#include "../IRAssignSt.hpp"

#include <memory>
#include <iostream>
#include <format>
#include <string>

IRAssignSt::IRAssignSt(IRNodeType ntype) : IRStatement(ntype) {}

const IRId* IRAssignSt::getVariable() const noexcept {
    return variable.get();
}

const IRExpression* IRAssignSt::getExp() const noexcept {
    return exp.get();
}

const IRTemporary* IRAssignSt::getTemporaries() const noexcept {
    return temporaries.get();
}

void IRAssignSt::setAssignSt(std::unique_ptr<IRId> var, std::unique_ptr<IRExpression> _exp, std::unique_ptr<IRTemporary> temp){
    variable = std::move(var);
    exp = std::move(_exp);
    temporaries = std::move(temp);
}

bool IRAssignSt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}

void IRAssignSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    if(temporaries != nullptr){
        temporaries->print(offset + 1);
    }
    variable->print(offset + 1);
    exp->print(offset + 1);
}