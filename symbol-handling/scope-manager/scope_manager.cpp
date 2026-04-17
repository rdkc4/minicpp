#include "scope_manager.hpp"

semantics::ScopeManager::ScopeManager(semantics::SymbolTable& symTab) 
    : symbolTable{ symTab } {}

void semantics::ScopeManager::pushScope(){
    scope.push({});
}

void semantics::ScopeManager::popScope(){
    auto& out{ scope.top() };
    // deleting symbols that went out of scope from symbol table
    while(!out.empty()){
        symbolTable.deleteSymbol(out.top());
        out.pop();
    }
    scope.pop();
}

bool semantics::ScopeManager::pushSymbol(const semantics::Symbol& symbol){
    if(symbolTable.insertSymbol(symbol.getName(), symbol)){
        scope.top().push(symbol.getName());
        return true;
    }
    return false;
}

const semantics::SymbolTable& semantics::ScopeManager::getSymbolTable() const noexcept {
    return symbolTable;
}

semantics::Symbol& semantics::ScopeManager::getSymbol(const std::string& name) const {
    return symbolTable.getSymbol(name);
}

const semantics::Symbol* semantics::ScopeManager::lookupSymbol(
    const std::string& name, std::initializer_list<semantics::Kind> kinds
) const {
    return symbolTable.lookupSymbol(name, kinds);
}

void semantics::ScopeManager::clear() noexcept {
    while(!scope.empty()){
        scope.pop();
    }
    symbolTable.clearSymbols();
}