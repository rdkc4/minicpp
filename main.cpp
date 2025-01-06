#include <iostream>

#include "symbol-handling/symbol-table/symbol_table.hpp"
#include "parser/parser.hpp"


int main(){
    std::string input = "";
    std::string line;

    while (std::getline(std::cin, line)) {
        input += line + "\n";
    }

    Lexer lexer(input);
    SymbolTable symbolTable;
    ScopeManager scopeManager(symbolTable);
    Parser parser(lexer, scopeManager);

    try {
        parser.parseProgram();
        std::cout << "\nProgram compiled successfully!\n";
    } catch (const std::exception& e) {
        std::cerr << "\n" << e.what() << "\n";
    }
    
    return 0;
}
