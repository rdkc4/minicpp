#include "scope_manager.hpp"

semantic::ScopeManager::ScopeManager(semantic::SymbolTable& symTab) 
    : symbolTable{ symTab } {}

void semantic::ScopeManager::pushScope(){
    scope.push({});
}

void semantic::ScopeManager::popScope(){
    auto& out{ scope.top() };
    // deleting symbols that went out of scope from symbol table
    while(!out.empty()){
        symbolTable.deleteSymbol(out.top());
        out.pop();
    }
    scope.pop();
}

bool semantic::ScopeManager::pushSymbol(const semantic::Symbol& symbol){
    if(symbolTable.insertSymbol(symbol.getName(), symbol)){
        scope.top().push(symbol.getName());
        return true;
    }
    return false;
}

const semantic::SymbolTable& semantic::ScopeManager::getSymbolTable() const noexcept {
    return symbolTable;
}

semantic::Symbol& semantic::ScopeManager::getSymbol(const std::string& name) const {
    return symbolTable.getSymbol(name);
}

const semantic::Symbol* semantic::ScopeManager::lookupSymbol(
    const std::string& name, std::initializer_list<semantic::Kind> kinds
) const {
    return symbolTable.lookupSymbol(name, kinds);
}

void semantic::ScopeManager::clear() noexcept {
    while(!scope.empty()){
        scope.pop();
    }
    symbolTable.clearSymbols();
}