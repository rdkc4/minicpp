#include "symbol_table.hpp"

#include <initializer_list>
#include <iostream>

bool SymbolTable::lookupSymbol(const std::string& name, std::initializer_list<Kinds> kind) const {
    auto it{ symbolTable.find(name) };
    if(it != symbolTable.end()){
        for(const auto& _kind : kind){
            if(it->second.getKind() == _kind){
                return true;
            }
        }
    }
    return false;
}

bool SymbolTable::insertSymbol(const std::string& name, const Symbol& symbol){
    auto result{ symbolTable.insert({name, symbol}) };
    return result.second; // false if symbol was already in symbol table
}

Symbol& SymbolTable::getSymbol(const std::string& name){
    return symbolTable.at(name);
}

void SymbolTable::deleteSymbol(const std::string& name){
    symbolTable.erase(name);
}

void SymbolTable::clearSymbols() noexcept {
    symbolTable.clear();
}

void SymbolTable::printSymbolTable() const {
    for(const auto& symbol : symbolTable){
        std::cout << symbol.second.symbolToString();
    }
}
