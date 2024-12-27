#include "../include/symbol.hpp"

Symbol::Symbol(std::string& name, Kinds kind, Types type, unsigned varnum)
    : name(name), kind(kind), type(type), variableNumber(varnum), parameters(nullptr){}

std::string Symbol::getName() const{
    return name;
}

Kinds Symbol::getKind() const{
    return kind;
}

Types Symbol::getType() const{
    return type;
}

unsigned Symbol::getVariableNumber() const{
    return variableNumber;
}

std::shared_ptr<ASTree> Symbol::getParameters() const{
    return parameters;
}

void Symbol::setName(const std::string& _name){
    name = _name;
}

void Symbol::setKind(const Kinds& _kind){
    kind = _kind;
}

void Symbol::setType(const Types& _type){
    type = _type;
}

void Symbol::setVariableNum(const unsigned varNum){
    variableNumber = varNum;
}

void Symbol::setParameters(std::shared_ptr<ASTree> par){
    parameters = par;
}

std::string Symbol::symbolToString(){
    return name + " | " + kindToString.at(kind) + " | " + typeToString.at(type) + " | " 
        + std::to_string(variableNumber) + " | " + std::to_string(parameters != nullptr ? parameters->getChildren().size() : 0) + "\n";
}