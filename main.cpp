#include <iostream>
#include <fstream>

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
    Parser parser(lexer);

    try {
        parser.parseProgram();  // Start parsing the program
        std::cout << "Program parsed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
