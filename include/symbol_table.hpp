#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <unordered_map>
#include <vector>
#include <memory>

#include "symbol.hpp"

class SymbolTable{
    public:

        bool lookupSymbol(const std::string& name, std::vector<Kinds> kind);

        bool insertSymbol(const std::string& name, std::shared_ptr<Symbol> symbol);

        std::shared_ptr<Symbol> getSymbol(const std::string& name);

        void deleteSymbol(const std::string& name);

        void clearSymbols();

        void printSymbolTable();

    private:

        std::unordered_map<std::string, std::shared_ptr<Symbol>> symbolTable;

};

#endif