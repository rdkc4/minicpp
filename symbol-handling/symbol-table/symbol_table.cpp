#include "symbol_table.hpp"

const semantic::Symbol* semantic::SymbolTable::lookupSymbol(
    const std::string& name, std::initializer_list<semantic::Kind> kinds
) const {
    auto it{ symbolTable.find(name) };
    if(it != symbolTable.end()){
        for(const auto& kind : kinds){
            if(it->second.getKind() == kind){
                return &it->second;
            }
        }
    }
    return nullptr;
}

bool semantic::SymbolTable::insertSymbol(const std::string& name, const semantic::Symbol& symbol){
    auto result{ symbolTable.insert({name, symbol}) };
    return result.second; // false if symbol was already in symbol table
}

semantic::Symbol& semantic::SymbolTable::getSymbol(const std::string& name){
    return symbolTable.at(name);
}

void semantic::SymbolTable::deleteSymbol(const std::string& name){
    symbolTable.erase(name);
}

void semantic::SymbolTable::clearSymbols() noexcept {
    symbolTable.clear();
}
