#include "../IRParameter.hpp"

#include <string>
#include <string_view>
#include <format>
#include <iostream>

IRParameter::IRParameter(IRNodeType ntype, std::string_view parName, Types type) : IRNode(ntype), parName{ parName }, type{ type } {}

const std::string& IRParameter::getParName() const noexcept {
    return parName;
}

void IRParameter::setParName(const std::string& _parName){
    parName = _parName;
}

Types IRParameter::getType() const noexcept {
    return type;
}

void IRParameter::setType(Types t) noexcept {
    type = t;
}

void IRParameter::print(size_t offset) const {
    std::cout << std::format("{}|-> {} | {}", std::string(offset*2, ' '), parName, toString());
}