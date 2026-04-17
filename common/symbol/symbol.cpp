#include "symbol.hpp"

#include <format>

semantics::Symbol::Symbol(std::string_view name, Kind kind, Type type) 
    : name{ name }, parameters{ nullptr }, kind{ kind }, type{ type } {}

const std::string& semantics::Symbol::getName() const noexcept {
    return name;
}

Kind semantics::Symbol::getKind() const noexcept {
    return kind;
}

Type semantics::Symbol::getType() const noexcept {
    return type;
}

const std::vector<std::unique_ptr<syntax::ast::ASTParameter>>* 
semantics::Symbol::getParameters() const noexcept {
    return parameters;
}

void semantics::Symbol::setName(std::string_view symName){
    name = symName;
}

void semantics::Symbol::setKind(Kind symKind) noexcept {
    kind = symKind;
}

void semantics::Symbol::setType(Type symType) noexcept {
    type = symType;
}

void semantics::Symbol::setParameters(
    const std::vector<std::unique_ptr<syntax::ast::ASTParameter>>* symParams
) noexcept {
    parameters = symParams;
}

std::string semantics::Symbol::symbolToString() const {
    return std::format("| {:11} | {:3} | {:8} | {:2} |\n", 
        name, 
        kindToStr(kind), 
        typeToStr(type), 
        (parameters != nullptr 
            ? std::to_string(parameters->size()) 
            : ""
        ));
}