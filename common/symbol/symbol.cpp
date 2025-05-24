#include "symbol.hpp"

#include <format>

Symbol::Symbol(std::string_view name, Kinds kind, Types type) : name{name}, kind{kind}, type{type}, parameters{nullptr} {}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// GETTERS
//-----------------------------------------------------------------------------------------------------------------------------------------------------

const std::string& Symbol::getName() const{
    return name;
}

const Kinds& Symbol::getKind() const{
    return kind;
}

const Types& Symbol::getType() const{
    return type;
}

ASTree* Symbol::getParameters() const{
    return parameters;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SETTERS
//-----------------------------------------------------------------------------------------------------------------------------------------------------

void Symbol::setName(std::string_view _name){
    name = _name;
}

void Symbol::setKind(Kinds _kind){
    kind = _kind;
}

void Symbol::setType(Types _type){
    type = _type;
}

void Symbol::setParameters(ASTree* par){
    parameters = par;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// PRINT SYMBOL
//-----------------------------------------------------------------------------------------------------------------------------------------------------
const std::string Symbol::symbolToString() const {
    return std::format("| {:11} | {:3} | {:8} | {:1} |\n", 
        name, kindToString.at(kind), typeToString.at(type), (parameters != nullptr ? parameters->getChildren().size() : 0));
}