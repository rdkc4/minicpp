#include "scope_manager.hpp"

Sym::ScopeManager::ScopeManager(Sym::SymbolTable& symTab) 
    : symbolTable{ symTab } {}

void Sym::ScopeManager::pushScope(){
    scope.push({});
}

void Sym::ScopeManager::popScope(){
    auto& out{ scope.top() };
    // deleting symbols that went out of scope from symbol table
    while(!out.empty()){
        symbolTable.deleteSymbol(out.top());
        out.pop();
    }
    scope.pop();
}

bool Sym::ScopeManager::pushSymbol(const Sym::defs::Symbol& symbol){
    if(symbolTable.insertSymbol(symbol.getName(), symbol)){
        scope.top().push(symbol.getName());
        return true;
    }
    return false;
}

const Sym::SymbolTable& Sym::ScopeManager::getSymbolTable() const noexcept {
    return symbolTable;
}

Sym::defs::Symbol& Sym::ScopeManager::getSymbol(const std::string& name) const {
    return symbolTable.getSymbol(name);
}

const Sym::defs::Symbol* Sym::ScopeManager::lookupSymbol(
    const std::string& name, std::initializer_list<Kind> kinds
) const {
    return symbolTable.lookupSymbol(name, kinds);
}

void Sym::ScopeManager::clear() noexcept {
    while(!scope.empty()){
        scope.pop();
    }
    symbolTable.clearSymbols();
}