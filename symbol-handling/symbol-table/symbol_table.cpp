#include "symbol_table.hpp"

const Sym::defs::Symbol* Sym::SymbolTable::lookupSymbol(
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

bool Sym::SymbolTable::insertSymbol(const std::string& name, const Sym::defs::Symbol& symbol){
    auto result{ symbolTable.insert({name, symbol}) };
    return result.second; // false if symbol was already in symbol table
}

Sym::defs::Symbol& Sym::SymbolTable::getSymbol(const std::string& name){
    return symbolTable.at(name);
}

void Sym::SymbolTable::deleteSymbol(const std::string& name){
    symbolTable.erase(name);
}

void Sym::SymbolTable::clearSymbols() noexcept {
    symbolTable.clear();
}
