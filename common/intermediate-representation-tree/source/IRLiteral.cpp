#include "../IRLiteral.hpp"

IRLiteral::IRLiteral(IRNodeType ntype, const std::string& val, Types type) : IRExpression(ntype, type), value{ val } {}

const std::string& IRLiteral::getValue() const noexcept {
    return value;
}

void IRLiteral::setValue(const std::string& val){
    value = val;
}