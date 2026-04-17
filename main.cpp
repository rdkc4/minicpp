#include <chrono>
#include <exception>
#include <iostream>
#include <format>

#include "compiler/compiler.hpp"

int main(int argc, char** argv){
    try {
        compiler::CompileOptions options{ compiler::parseOptions(argc, argv) };
        auto start{ std::chrono::high_resolution_clock::now() };

        compiler::ExitCode ret{ compiler::compile(options) };

        if(ret != compiler::ExitCode::NO_ERR){
            std::cerr << "Program failed to compile!\n";
            return 1;
        }

        auto duration{ 
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - start
            )
        };

        std::cout << std::format("Compilation time: {}ms\n", duration.count());
        return 0;

    }
    catch(const std::exception& e){
        std::cerr << std::format("error: {}\n", e.what());
        return 1;
    }
}
