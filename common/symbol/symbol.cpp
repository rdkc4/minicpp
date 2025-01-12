#include "symbol.hpp"

#include <format>

Symbol::Symbol(std::string& name, Kinds kind, Types type)
    : name(name), kind(kind), type(type), parameters(nullptr){}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// GETTERS
//-----------------------------------------------------------------------------------------------------------------------------------------------------

std::string Symbol::getName() const{
    return name;
}

Kinds Symbol::getKind() const{
    return kind;
}

Types Symbol::getType() const{
    return type;
}

std::shared_ptr<ASTree> Symbol::getParameters() const{
    return parameters;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SETTERS
//-----------------------------------------------------------------------------------------------------------------------------------------------------

void Symbol::setName(const std::string& _name){
    name = _name;
}

void Symbol::setKind(const Kinds& _kind){
    kind = _kind;
}

void Symbol::setType(const Types& _type){
    type = _type;
}

void Symbol::setParameters(std::shared_ptr<ASTree> par){
    parameters = par;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// PRINT SYMBOL
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::string Symbol::symbolToString(){
    return std::format("| {:11} | {:3} | {:8} | {:1} |\n", 
        name, kindToString.at(kind), typeToString.at(type), (parameters != nullptr ? parameters->getChildren().size() : 0));
}