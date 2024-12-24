#include "../include/symbol_table.hpp"

bool SymbolTable::lookupSymbol(const std::string& name, std::vector<Kinds> kind){
    auto it = symbolTable.find(name);
    if(it != symbolTable.end()){
        for(const auto& _kind : kind){
            if(it->second.getKind() == _kind){
                return true;
            }
        }
    }
    return false;
}

bool SymbolTable::insertSymbol(const std::string& name, Symbol symbol){
    auto result = symbolTable.insert({name, symbol});
    if(!result.second){
        return false;
    }
    return true;
}

void SymbolTable::deleteSymbol(const std::string& name){
    symbolTable.erase(name);
}

void SymbolTable::clearSymbols(){
    symbolTable.clear();
}