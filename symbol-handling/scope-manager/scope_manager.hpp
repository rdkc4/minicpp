#ifndef SCOPE_MANAGER_HPP
#define SCOPE_MANAGER_HPP

#include <stack>
#include <initializer_list>

#include "../symbol-table/symbol_table.hpp"

class ScopeManager{
    public:

        ScopeManager(SymbolTable& symTab);

        // scope operations
        void pushScope(); // entered new scope
        void popScope(); // exited scope

        // symbol table operations
        bool pushSymbol(const Symbol& symbol); // new definition in current scope
        const SymbolTable& getSymbolTable() const noexcept;
        Symbol& getSymbol(const std::string& name) const;
        bool lookupSymbol(const std::string& name, std::initializer_list<Kinds> kind) const;

        void printSymbolTable() const;

    private:
        // table of active symbols
        SymbolTable& symbolTable;
        // stack of scopes
        std::stack<std::stack<std::string>> scope;

};

#endif
