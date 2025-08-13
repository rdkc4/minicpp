#include "../IRId.hpp"

#include <string>
#include <string_view>
#include <iostream>
#include <format>

IRId::IRId(IRNodeType ntype, std::string_view idName, Types type) : IRExpression(ntype, type), idName{ idName }, value{ "0" } {}

const std::string& IRId::getIdName() const noexcept {
    return idName;
}

void IRId::setIdName(const std::string& _idName){
    idName = _idName;
}

const std::string& IRId::getValue() const noexcept {
    return value;
}

void IRId::setValue(const std::string& val){
    value = val;
}

void IRId::print(size_t offset) const {
    std::cout << std::format("{}|-> {} | {} | {}", std::string(offset*2, ' '), idName, value, toString());
}