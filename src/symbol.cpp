#include "../include/symbol.hpp"

Symbol::Symbol(std::string& name, Kinds kind, Types type, unsigned atr1, unsigned atr2)
    : name(name), kind(kind), type(type), atribute1(atr1), atribute2(atr2) {}

std::string Symbol::getName() const{
    return name;
}

Kinds Symbol::getKind() const{
    return kind;
}

Types Symbol::getType() const{
    return type;
}

unsigned Symbol::getAtribute1() const{
    return atribute1;
}

unsigned Symbol::getAtribute2() const{
    return atribute2;
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

void Symbol::setAtribute1(const unsigned atr1){
    atribute1 = atr1;
}

void Symbol::setAtribute2(const unsigned atr2){
    atribute2 = atr2;
}

std::string Symbol::kindToString(Kinds kind){
    switch(kind){
        case Kinds::REG:
            return "REG";
        case Kinds::LIT:
            return "LIT";
        case Kinds::FUN:
            return "FUN";
        case Kinds::VAR:
            return "VAR";
        case Kinds::PAR:
            return "PAR";
        default:
            return "NO_KIND";
    }
}

std::string Symbol::typeToString(Types type){
    switch(type){
        case Types::INT:
            return "INT";
        case Types::UNSIGNED:
            return "UNSIGNED";
        case Types::VOID:
            return "VOID";
        default:
            return "NO_TYPE";
    }
}


std::string Symbol::symbolToString(){
    return name + " | " + kindToString(kind) + " | " + typeToString(type) + " | " + std::to_string(atribute1) + " | " + std::to_string(atribute2) + "\n";
}