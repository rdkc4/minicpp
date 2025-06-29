#include <chrono>
#include <iostream>
#include <format>

#include "compiler/compiler.hpp"

int main(){
    std::string input { "" };
    std::string line {};

    while (std::getline(std::cin, line)) {
        input += line + "\n";
    }

    std::string output{ "output.s" };

    Compiler compiler;

    auto start{ std::chrono::high_resolution_clock::now() };

    auto ret{ compiler.compile(input, output) };

    auto end{ std::chrono::high_resolution_clock::now() };
    auto duration{ std::chrono::duration_cast<std::chrono::milliseconds>(end - start) };
    std::cout << std::format("\nCompilation time: {}ms\n", duration.count());
    
    if(ret != ExitCode::NO_ERR){
        std::cerr << "\nProgram failed to compile!\n";
    }

    return 0;
}
