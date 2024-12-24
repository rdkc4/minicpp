#include <iostream>
#include <fstream>

#include "include/defs.hpp"
#include "include/symbol_table.hpp"
#include "include/parser.hpp"


int main(){
    std::string filePath = "testfile.txt";
    std::ifstream file(filePath);
    if(!file){
        std::cout << "Invalid file path!\n";
        return -1;
    }
    std::string input = std::string(std::istreambuf_iterator<char>(file), {});
    Lexer lexer(input);
    SymbolTable symbolTable;
    ScopeManager scopeManager(symbolTable);
    Parser parser(lexer, scopeManager);

    try {
        parser.parseProgram();
        std::cout << "Program parsed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
