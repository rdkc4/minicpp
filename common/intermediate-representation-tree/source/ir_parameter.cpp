#include "../ir_parameter.hpp"

#include "../defs/ir_defs.hpp"

IRParameter::IRParameter(std::string_view parName, Type type) : IRNode(IRNodeType::PARAMETER), parameterName{ parName }, type{ type } {}

const std::string& IRParameter::getParameterName() const noexcept {
    return parameterName;
}

void IRParameter::setParameterName(const std::string& parName){
    parameterName = parName;
}

Type IRParameter::getType() const noexcept {
    return type;
}

void IRParameter::setType(Type parameterType) noexcept {
    type = parameterType;
}

void IRParameter::accept(IRVisitor& visitor){
    visitor.visit(this);
}