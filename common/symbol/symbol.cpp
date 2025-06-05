#include "symbol.hpp"

#include <format>

Symbol::Symbol(std::string_view name, Kinds kind, Types type) : name{ name }, parameters{ nullptr }, kind{ kind }, type{ type } {}

const std::string& Symbol::getName() const noexcept {
    return name;
}

Kinds Symbol::getKind() const noexcept {
    return kind;
}

Types Symbol::getType() const noexcept {
    return type;
}

ASTree* Symbol::getParameters() const noexcept {
    return parameters;
}

void Symbol::setName(std::string_view _name){
    name = _name;
}

void Symbol::setKind(Kinds _kind) noexcept {
    kind = _kind;
}

void Symbol::setType(Types _type) noexcept {
    type = _type;
}

void Symbol::setParameters(ASTree* par) noexcept {
    parameters = par;
}

std::string Symbol::symbolToString() const {
    return std::format("| {:11} | {:3} | {:8} | {:1} |\n", 
        name, kindToString.at(kind), typeToString.at(type), (parameters != nullptr ? parameters->getChildren().size() : 0));
}