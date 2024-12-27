#ifndef SCOPE_MANAGER_HPP
#define SCOPE_MANAGER_HPP

#include <stack>

#include "symbol_table.hpp"

class ScopeManager{
    public:

        ScopeManager(SymbolTable& symTab);

        void pushScope();

        void popScope();

        bool pushSymbol(const std::shared_ptr<Symbol> symbol);

        void printSymbolTable();

        SymbolTable getSymbolTable();

    private:

        SymbolTable& symbolTable;
        std::stack<std::stack<std::string>> scope;

};

#endif