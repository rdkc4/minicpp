#include "../IRId.hpp"

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