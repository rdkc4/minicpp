#include "symbol.hpp"

#include <format>

Sym::defs::Symbol::Symbol(std::string_view name, Kind kind, Type type) 
    : name{ name }, parameters{ nullptr }, kind{ kind }, type{ type } {}

const std::string& Sym::defs::Symbol::getName() const noexcept {
    return name;
}

Kind Sym::defs::Symbol::getKind() const noexcept {
    return kind;
}

Type Sym::defs::Symbol::getType() const noexcept {
    return type;
}

const std::vector<std::unique_ptr<AST::node::ASTParameter>>* 
Sym::defs::Symbol::getParameters() const noexcept {
    return parameters;
}

void Sym::defs::Symbol::setName(std::string_view symName){
    name = symName;
}

void Sym::defs::Symbol::setKind(Kind symKind) noexcept {
    kind = symKind;
}

void Sym::defs::Symbol::setType(Type symType) noexcept {
    type = symType;
}

void Sym::defs::Symbol::setParameters(
    const std::vector<std::unique_ptr<AST::node::ASTParameter>>* symParams
) noexcept {
    parameters = symParams;
}

std::string Sym::defs::Symbol::symbolToString() const {
    return std::format("| {:11} | {:3} | {:8} | {:2} |\n", 
        name, 
        kindToStr(kind), 
        typeToStr(type), 
        (parameters != nullptr 
            ? std::to_string(parameters->size()) 
            : ""
        ));
}