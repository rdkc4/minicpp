#include "../include/scope_manager.hpp"

ScopeManager::ScopeManager(SymbolTable& symTab) : symbolTable(symTab){}

void ScopeManager::pushScope(){
    scope.push({});
}

void ScopeManager::popScope(){
    auto out = scope.top();
    while(!out.empty()){
        symbolTable.deleteSymbol(out.top());
        out.pop();
    }
    scope.pop();
}

bool ScopeManager::pushSymbol(const Symbol& symbol){
    if(symbolTable.insertSymbol(symbol.getName(), symbol)){
        scope.top().push(symbol.getName());
        return true;
    }
    return false;
}

void ScopeManager::printSymbolTable(){
    symbolTable.printSymbolTable();
    std::cout << std::string(20,'-') << "\n";
}