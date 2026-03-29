#include "scope_manager.hpp"

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

const Symbol* ScopeManager::lookupSymbol(const std::string& name, std::initializer_list<Kind> kind) const {
    return symbolTable.lookupSymbol(name, kind);
}

void ScopeManager::clear() noexcept {
    while(!scope.empty()){
        scope.pop();
    }
    symbolTable.clearSymbols();
}