#include <chrono>
#include <fstream>
#include <iostream>
#include <format>
#include <sstream>

#include "compiler/compiler.hpp"

int main(int argc, char** argv){
    if(argc < 2 || argc > 3) {
        std::cerr << std::format("Usage: {} <source-file> (<output-file>)\n", argv[0]);
        return 1;
    }

    std::string inputFile = argv[1];
    std::ifstream inputFileStream(inputFile);

    if(!inputFileStream.is_open()){
        std::cerr << std::format("Failed to open file \"{}\"\n", inputFile);
        return 2;
    }

    std::stringstream input;
    input << inputFileStream.rdbuf();

    std::string output{ argc == 3 ? argv[2] : "output.s" };

    auto start{ std::chrono::high_resolution_clock::now() };

    auto ret{ Compiler::compile(input.str(), output) };

    auto end{ std::chrono::high_resolution_clock::now() };
    auto duration{ std::chrono::duration_cast<std::chrono::milliseconds>(end - start) };

    if(ret != Compiler::ExitCode::NO_ERR){
        std::cerr << "\nProgram failed to compile!\n";
    }
    std::cout << std::format("\nCompilation time: {}ms\n", duration.count());

    return 0;
}
