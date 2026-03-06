#include "../ir_parameter.hpp"

IRParameter::IRParameter(IRNodeType ntype, std::string_view parName, Type type) : IRNode(ntype), parName{ parName }, type{ type } {}

const std::string& IRParameter::getParName() const noexcept {
    return parName;
}

void IRParameter::setParName(const std::string& _parName){
    parName = _parName;
}

Type IRParameter::getType() const noexcept {
    return type;
}

void IRParameter::setType(Type t) noexcept {
    type = t;
}