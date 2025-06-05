#include "scope_manager.hpp"

#include <iostream>
#include <initializer_list>

ScopeManager::ScopeManager(SymbolTable& symTab) : symbolTable{ symTab } {}

void ScopeManager::pushScope(){
    scope.push({});
}

void ScopeManager::popScope(){
    auto& out{ scope.top() };
    // deleting symbols that went out of scope from symbol table
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

const SymbolTable& ScopeManager::getSymbolTable() const noexcept {
    return symbolTable;
}

Symbol& ScopeManager::getSymbol(const std::string& name) const {
    return symbolTable.getSymbol(name);
}

bool ScopeManager::lookupSymbol(const std::string& name, std::initializer_list<Kinds> kind) const {
    return symbolTable.lookupSymbol(name, kind);
}

void ScopeManager::printSymbolTable() const {
    symbolTable.printSymbolTable();
    std::cout << std::string(36,'-') << "\n";
}
