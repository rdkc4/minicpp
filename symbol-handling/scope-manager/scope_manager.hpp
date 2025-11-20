#ifndef SCOPE_MANAGER_HPP
#define SCOPE_MANAGER_HPP

#include <stack>
#include <initializer_list>

#include "../symbol-table/symbol_table.hpp"

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
     * @returns void
    */
    void pushScope();

    /** 
     * @brief removing a stack when scope is exited and clearing symbols that are expired
     * @returns void
    */
    void popScope();

    /** 
     * @brief inserts symbol into symbol table, and adds its key to the most recent scope
     * @param symbol - symbol that is being inserted into symbol table
     * @returns true if symbol is inserted, false if symbol already exists
    */
    bool pushSymbol(const Symbol& symbol);

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
    Symbol& getSymbol(const std::string& name) const;

    /** 
     * @brief checks if symbol exists in a table
     * @param name - name of the symbol
     * @param kind - list of kinds that symbol is allowed to have
     * @returns true if symbol of given name and kind exists, false otherwise
    */
    bool lookupSymbol(const std::string& name, std::initializer_list<Kinds> kind) const;

    /** 
     * @brief prints all symbols from the symbol table
     * @note debugging purposes
     * @returns void
    */
    void printSymbolTable() const;

private:
    /// table of active symbols
    SymbolTable& symbolTable;
    /// stack of scopes
    std::stack<std::stack<std::string>> scope;

};

#endif
