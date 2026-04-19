#include "symbol.hpp"

#include <format>

semantic::Symbol::Symbol(std::string_view name, semantic::Kind kind, types::Type type) 
    : name{ name }, parameters{ nullptr }, kind{ kind }, type{ type } {}

const std::string& semantic::Symbol::getName() const noexcept {
    return name;
}

semantic::Kind semantic::Symbol::getKind() const noexcept {
    return kind;
}

types::Type semantic::Symbol::getType() const noexcept {
    return type;
}

const std::vector<std::unique_ptr<syntax::ast::ASTParameter>>* 
semantic::Symbol::getParameters() const noexcept {
    return parameters;
}

void semantic::Symbol::setName(std::string_view symName){
    name = symName;
}

void semantic::Symbol::setKind(semantic::Kind symKind) noexcept {
    kind = symKind;
}

void semantic::Symbol::setType(types::Type symType) noexcept {
    type = symType;
}

void semantic::Symbol::setParameters(
    const std::vector<std::unique_ptr<syntax::ast::ASTParameter>>* symParams
) noexcept {
    parameters = symParams;
}

std::string semantic::Symbol::symbolToString() const {
    return std::format("| {:11} | {:3} | {:8} | {:2} |\n", 
        name, 
        semantic::kindToStr(kind), 
        typeToStr(type), 
        (parameters != nullptr 
            ? std::to_string(parameters->size()) 
            : ""
        ));
}