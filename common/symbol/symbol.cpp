#include "symbol.hpp"

#include <format>

sym::Symbol::Symbol(std::string_view name, Kind kind, Type type) 
    : name{ name }, parameters{ nullptr }, kind{ kind }, type{ type } {}

const std::string& sym::Symbol::getName() const noexcept {
    return name;
}

Kind sym::Symbol::getKind() const noexcept {
    return kind;
}

Type sym::Symbol::getType() const noexcept {
    return type;
}

const std::vector<std::unique_ptr<syntax::ast::ASTParameter>>* 
sym::Symbol::getParameters() const noexcept {
    return parameters;
}

void sym::Symbol::setName(std::string_view symName){
    name = symName;
}

void sym::Symbol::setKind(Kind symKind) noexcept {
    kind = symKind;
}

void sym::Symbol::setType(Type symType) noexcept {
    type = symType;
}

void sym::Symbol::setParameters(
    const std::vector<std::unique_ptr<syntax::ast::ASTParameter>>* symParams
) noexcept {
    parameters = symParams;
}

std::string sym::Symbol::symbolToString() const {
    return std::format("| {:11} | {:3} | {:8} | {:2} |\n", 
        name, 
        kindToStr(kind), 
        typeToStr(type), 
        (parameters != nullptr 
            ? std::to_string(parameters->size()) 
            : ""
        ));
}