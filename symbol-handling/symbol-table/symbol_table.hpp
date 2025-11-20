#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <unordered_map>
#include <initializer_list>

#include "../../common/symbol/symbol.hpp"

/** 
 * @class SymbolTable
 * @brief maintaining the table of symbols
*/
class SymbolTable{
public:
    /** 
     * @brief Creates the instance of the symbol table
    */
    SymbolTable();

    /** 
     * @brief checks if symbol exists in a table
     * @param name - name of the symbol
     * @param kind - list of kinds that symbol is allowed to have
     * @returns true if symbol of given name and kind exists, false otherwise
    */
    bool lookupSymbol(const std::string& name, std::initializer_list<Kinds> kind) const; // check if symbol of given kind exists

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
     * @returns void
    */
    void deleteSymbol(const std::string& name);

    /** 
     * @brief removes all symbols from the symbol table
     * @returns void
    */
    void clearSymbols() noexcept;

    /** 
     * @brief prints all symbols from the symbol table
     * @note debugging purposes
     * @returns void
    */
    void printSymbolTable() const;

private:
    /// mapping symbol name to symbol
    std::unordered_map<std::string, Symbol> symbolTable;

};

#endif
