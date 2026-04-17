#include "symbol_table.hpp"

const semantics::Symbol* semantics::SymbolTable::lookupSymbol(
    const std::string& name, std::initializer_list<Kind> kinds
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

bool semantics::SymbolTable::insertSymbol(const std::string& name, const semantics::Symbol& symbol){
    auto result{ symbolTable.insert({name, symbol}) };
    return result.second; // false if symbol was already in symbol table
}

semantics::Symbol& semantics::SymbolTable::getSymbol(const std::string& name){
    return symbolTable.at(name);
}

void semantics::SymbolTable::deleteSymbol(const std::string& name){
    symbolTable.erase(name);
}

void semantics::SymbolTable::clearSymbols() noexcept {
    symbolTable.clear();
}
