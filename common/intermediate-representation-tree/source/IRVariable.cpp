#include "../IRVariable.hpp"

#include <memory>
#include <iostream>
#include <format>
#include <string>

IRVariable::IRVariable(IRNodeType ntype, std::string_view varName, Types type) : IRStatement(ntype), varName{ varName }, value{ "0" }, type{ type } {}

const IRExpression* IRVariable::getAssign() const noexcept {
    return assignment.get();
}

void IRVariable::setAssign(std::unique_ptr<IRExpression> assign, std::unique_ptr<IRTemporary> temp){
    assignment = std::move(assign);
    temporaries = std::move(temp);
}

const std::string& IRVariable::getVarName() const noexcept {
    return varName;
}

void IRVariable::setVarName(const std::string& var){
    varName = var;
}

const std::string& IRVariable::getValue() const noexcept {
    return value;
}

void IRVariable::setValue(const std::string& val){
    value = val;
}

Types IRVariable::getType() const noexcept {
    return type;
}

void IRVariable::setType(Types t) noexcept {
    type = t;
}

bool IRVariable::hasAssign() const noexcept {
    return assignment != nullptr;
}

const IRTemporary* IRVariable::getTemporaries() const noexcept {
    return temporaries.get();
}

bool IRVariable::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}

void IRVariable::print(size_t offset) const {
    std::cout << std::format("{}|-> {} | {} | {}", std::string(offset*2, ' '), varName, value, toString());
    if(hasAssign()){
        if(temporaries != nullptr){
            temporaries->print(offset + 1);
        }
        assignment->print(offset + 1);
    }
}