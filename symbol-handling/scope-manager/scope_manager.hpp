#ifndef SCOPE_MANAGER_HPP
#define SCOPE_MANAGER_HPP

#include <stack>

#include "../symbol-table/symbol_table.hpp"

class ScopeManager{
    public:

        ScopeManager(SymbolTable& symTab);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // scope operations
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void pushScope();

        void popScope();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // symbol table operations
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        bool pushSymbol(const std::shared_ptr<Symbol> symbol);

        SymbolTable getSymbolTable();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // display
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void printSymbolTable();

    private:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // symbol table - table of valid symbols
        // scope - stack of scopes
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        SymbolTable& symbolTable;
        std::stack<std::stack<std::string>> scope;

};

#endif
