#include "scope_manager.hpp"

#include <iostream>

ScopeManager::ScopeManager(SymbolTable& symTab) : symbolTable{symTab} {}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ENTERED NEW SCOPE
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void ScopeManager::pushScope(){
    scope.push({});
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SCOPE EXPIRED - DELETE EXPIRED SYMBOLS FROM SYMBOL TABLE
// -> allow variable declaration anywhere (TODO)  
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void ScopeManager::popScope(){
    auto out = scope.top();
    while(!out.empty()){
        symbolTable.deleteSymbol(out.top());
        out.pop();
    }
    scope.pop();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// INSERT SYMBOL FOR CURRENT SCOPE
//-----------------------------------------------------------------------------------------------------------------------------------------------------
bool ScopeManager::pushSymbol(const Symbol symbol){
    if(symbolTable.insertSymbol(symbol.getName(), symbol)){
        scope.top().push(symbol.getName());
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ACCESS SYMBOL TABLE
//-----------------------------------------------------------------------------------------------------------------------------------------------------
SymbolTable& ScopeManager::getSymbolTable() const {
    return symbolTable;
}

Symbol& ScopeManager::getSymbol(const std::string& name) const{
    return symbolTable.getSymbol(name);
}

bool ScopeManager::lookupSymbol(const std::string& name, std::vector<Kinds> kind) const{
    return symbolTable.lookupSymbol(name, kind);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// PRINT SYMBOL TABLE (debbuging purposes)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void ScopeManager::printSymbolTable() const {
    symbolTable.printSymbolTable();
    std::cout << std::string(36,'-') << "\n";
}
