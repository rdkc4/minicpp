#include "scope_manager.hpp"

sym::ScopeManager::ScopeManager(sym::SymbolTable& symTab) 
    : symbolTable{ symTab } {}

void sym::ScopeManager::pushScope(){
    scope.push({});
}

void sym::ScopeManager::popScope(){
    auto& out{ scope.top() };
    // deleting symbols that went out of scope from symbol table
    while(!out.empty()){
        symbolTable.deleteSymbol(out.top());
        out.pop();
    }
    scope.pop();
}

bool sym::ScopeManager::pushSymbol(const sym::Symbol& symbol){
    if(symbolTable.insertSymbol(symbol.getName(), symbol)){
        scope.top().push(symbol.getName());
        return true;
    }
    return false;
}

const sym::SymbolTable& sym::ScopeManager::getSymbolTable() const noexcept {
    return symbolTable;
}

sym::Symbol& sym::ScopeManager::getSymbol(const std::string& name) const {
    return symbolTable.getSymbol(name);
}

const sym::Symbol* sym::ScopeManager::lookupSymbol(
    const std::string& name, std::initializer_list<Kind> kinds
) const {
    return symbolTable.lookupSymbol(name, kinds);
}

void sym::ScopeManager::clear() noexcept {
    while(!scope.empty()){
        scope.pop();
    }
    symbolTable.clearSymbols();
}