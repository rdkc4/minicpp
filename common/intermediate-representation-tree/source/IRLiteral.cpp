#include "../IRLiteral.hpp"

#include <format>
#include <iostream>
#include <string>

IRLiteral::IRLiteral(IRNodeType ntype, const std::string& val, Types type) : IRExpression(ntype, type), value{ val } {}

const std::string& IRLiteral::getValue() const noexcept {
    return value;
}

void IRLiteral::setValue(const std::string& val){
    value = val;
}

void IRLiteral::print(size_t offset) const {
    std::cout << std::format("{}|-> {} | {}", std::string(offset*2, ' '), value, toString());
}