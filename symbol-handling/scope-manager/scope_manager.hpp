#ifndef SCOPE_MANAGER_HPP
#define SCOPE_MANAGER_HPP

#include <stack>
#include <initializer_list>

#include "../symbol-table/symbol_table.hpp"

namespace sym {
    /** 
     * @class ScopeManager
     * @brief maintains symbols that are in scope
    */
    class ScopeManager{
    public:
        /** 
         * @brief ScopeManager constructor
         * @param symTab - reference to a symbol table
        */
        ScopeManager(SymbolTable& symTab);

        /** 
         * @brief adding an empty stack when new scope is entered
        */
        void pushScope();

        /** 
         * @brief removing a stack when scope is exited and clearing symbols that are expired
        */
        void popScope();

        /** 
         * @brief inserts symbol into symbol table, and adds its key to the most recent scope
         * @param symbol - symbol that is being inserted into symbol table
         * @returns true if symbol is inserted, false if symbol already exists
        */
        bool pushSymbol(const sym::Symbol& symbol);

        /** 
         * @brief gets the symbol table
         * @returns constant reference to a symbol table 
        */
        const SymbolTable& getSymbolTable() const noexcept;

        /**
         * @brief gets the symbol from the symbol table
         * @param name - name of the requested symbol
         * @returns reference to a symbol with a given name
        */
        sym::Symbol& getSymbol(const std::string& name) const;

        /** 
         * @brief checks if symbol exists in a table
         * @param name - name of the symbol
         * @param kinds - list of kinds that symbol is allowed to have
         * @returns const pointer to symbol if symbol exists, nullptr otherwise
        */
        const sym::Symbol* lookupSymbol(const std::string& name, std::initializer_list<Kind> kinds) const;

        /**
         * @brief clears all scopes
        */
        void clear() noexcept;

    private:
        /// table of active symbols
        SymbolTable& symbolTable;
        
        /// stack of scopes
        std::stack<std::stack<std::string>> scope;

    };

}

#endif