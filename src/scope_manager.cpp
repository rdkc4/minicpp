#include "../include/scope_manager.hpp"

ScopeManager::ScopeManager(SymbolTable& symTab) : symbolTable(symTab){}

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
bool ScopeManager::pushSymbol(std::shared_ptr<Symbol> symbol){
    if(symbolTable.insertSymbol(symbol->getName(), symbol)){
        scope.top().push(symbol->getName());
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// PRINT SYMBOL TABLE (debbuging purposes)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void ScopeManager::printSymbolTable(){
    symbolTable.printSymbolTable();
    std::cout << std::string(20,'-') << "\n";
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ACCESS SYMBOL TABLE
//-----------------------------------------------------------------------------------------------------------------------------------------------------
SymbolTable ScopeManager::getSymbolTable(){
    return symbolTable;
}