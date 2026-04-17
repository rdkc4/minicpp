#include "../ir_parameter.hpp"

#include "../defs/ir_defs.hpp"

ir::IRParameter::IRParameter(std::string_view parName, Type type) 
    : IRNode(ir::IRNodeType::PARAMETER), 
      parameterName{ parName }, 
      type{ type } {}

const std::string& ir::IRParameter::getParameterName() const noexcept {
    return parameterName;
}

void ir::IRParameter::setParameterName(const std::string& parName){
    parameterName = parName;
}

Type ir::IRParameter::getType() const noexcept {
    return type;
}

void ir::IRParameter::setType(Type parameterType) noexcept {
    type = parameterType;
}

void ir::IRParameter::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}