#include "symbol_table.hpp"

#include <iostream>

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// CHECK IF SYMBOL EXISTS
//-----------------------------------------------------------------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// INSERT SYMBOL IF NOT DEFINED
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool SymbolTable::insertSymbol(const std::string& name, const Symbol symbol){
    auto result = symbolTable.insert({name, symbol});
    if(!result.second){
        return false;
    }
    return true;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETRIEVE SYMBOL ASSOCIATED WITH THE NAME
//-----------------------------------------------------------------------------------------------------------------------------------------------------
Symbol& SymbolTable::getSymbol(const std::string& name){
    return symbolTable.at(name);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DELETE THE SYMBOL ASSOCIATED WITH THE NAME
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void SymbolTable::deleteSymbol(const std::string& name){
    symbolTable.erase(name);
}

void SymbolTable::clearSymbols(){
    symbolTable.clear();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// PRINT SYMBOL TABLE (debbuging purposes)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void SymbolTable::printSymbolTable(){
    for(auto it = symbolTable.begin(); it != symbolTable.end(); it++){
        std::cout << it->second.symbolToString();
    }
}
