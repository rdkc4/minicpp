#include "../ir_parameter.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRParameter::IRParameter(std::string_view parName, Type type) 
    : IRNode(IR::defs::IRNodeType::PARAMETER), parameterName{ parName }, type{ type } {}

const std::string& IR::node::IRParameter::getParameterName() const noexcept {
    return parameterName;
}

void IR::node::IRParameter::setParameterName(const std::string& parName){
    parameterName = parName;
}

Type IR::node::IRParameter::getType() const noexcept {
    return type;
}

void IR::node::IRParameter::setType(Type parameterType) noexcept {
    type = parameterType;
}

void IR::node::IRParameter::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}