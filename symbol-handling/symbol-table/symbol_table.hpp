#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <unordered_map>
#include <initializer_list>

#include "../../common/symbol/symbol.hpp"

/** 
 * @class SymbolTable
 * @brief maintaining the table of symbols
*/
class SymbolTable {
public:
    /** 
     * @brief Creates the instance of the symbol table
    */
    SymbolTable() = default;

    /** 
     * @brief checks if symbol exists in a table
     * @param name - name of the symbol
     * @param kind - list of kinds that symbol is allowed to have
     * @returns const pointer to symbol if symbol exists, nullptr otherwise
    */
    const Symbol* lookupSymbol(const std::string& name, std::initializer_list<Kind> kind) const;

    /** 
     * @brief inserts symbol into the symbol table if the symbol with the given name doesn't exist
     * @param name - name of the symbol
     * @param symbol - constant reference to a symbol that should be inserted
     * @returns true if insertion is successful, false if symbol already exists
    */
    bool insertSymbol(const std::string& name, const Symbol& symbol);

    /** 
     * @brief gets the symbol from the symbol table
     * @param name - name of the requested symbol
     * @returns reference to a symbol with the given name
    */
    Symbol& getSymbol(const std::string& name);

    /** 
     * @brief removes symbol from symbol table
     * @param name - name of the symbol that should be deleted
    */
    void deleteSymbol(const std::string& name);

    /** 
     * @brief removes all symbols from the symbol table
    */
    void clearSymbols() noexcept;

private:
    /// mapping symbol name to symbol
    std::unordered_map<std::string, Symbol> symbolTable;

};

#endif
