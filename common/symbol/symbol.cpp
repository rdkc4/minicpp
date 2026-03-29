#include "symbol.hpp"

#include <format>

Symbol::Symbol(std::string_view name, Kind kind, Type type) : name{ name }, parameters{ nullptr }, kind{ kind }, type{ type } {}

const std::string& Symbol::getName() const noexcept {
    return name;
}

Kind Symbol::getKind() const noexcept {
    return kind;
}

Type Symbol::getType() const noexcept {
    return type;
}

const std::vector<std::unique_ptr<ASTParameter>>* Symbol::getParameters() const noexcept {
    return parameters;
}

void Symbol::setName(std::string_view symName){
    name = symName;
}

void Symbol::setKind(Kind symKind) noexcept {
    kind = symKind;
}

void Symbol::setType(Type symType) noexcept {
    type = symType;
}

void Symbol::setParameters(const std::vector<std::unique_ptr<ASTParameter>>* symParams) noexcept {
    parameters = symParams;
}

std::string Symbol::symbolToString() const {
    return std::format("| {:11} | {:3} | {:8} | {:2} |\n", 
        name, kindToString.at(kind), typeToString.at(type), (parameters != nullptr ? std::to_string(parameters->size()) : ""));
}