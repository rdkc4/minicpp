#include "symbol_table.hpp"

const sym::Symbol* sym::SymbolTable::lookupSymbol(
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

bool sym::SymbolTable::insertSymbol(const std::string& name, const sym::Symbol& symbol){
    auto result{ symbolTable.insert({name, symbol}) };
    return result.second; // false if symbol was already in symbol table
}

sym::Symbol& sym::SymbolTable::getSymbol(const std::string& name){
    return symbolTable.at(name);
}

void sym::SymbolTable::deleteSymbol(const std::string& name){
    symbolTable.erase(name);
}

void sym::SymbolTable::clearSymbols() noexcept {
    symbolTable.clear();
}
