#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <unordered_map>
#include <initializer_list>

#include "../../common/symbol/symbol.hpp"

class SymbolTable{
    public:

        // symbol table operations
        bool lookupSymbol(const std::string& name, std::initializer_list<Kinds> kind) const; // check if symbol of given kind exists
        bool insertSymbol(const std::string& name, const Symbol& symbol);

        Symbol& getSymbol(const std::string& name);

        void deleteSymbol(const std::string& name);
        void clearSymbols() noexcept;

        void printSymbolTable() const;

    private:
        // mapping symbol name to symbol
        std::unordered_map<std::string, Symbol> symbolTable;

};

#endif
