#include "../code_generator.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <latch>

#include "../../asm-generator/asm_instruction_generator.hpp"
#include "../function_code_generator.hpp"

code_gen::CodeGenerator::CodeGenerator(std::string_view filePath, util::concurrency::ThreadPool& threadPool) 
    : threadPool{ threadPool}, 
      outputPath{ filePath } {}

void code_gen::CodeGenerator::generateProgram(const ir::IRProgram* program){
    std::latch doneLatch{ static_cast<ptrdiff_t>(program->getFunctionCount()) };

    for(const auto& function : program->getFunctions()){
        threadPool.enqueue(
            [this, function=function.get(), &doneLatch] -> void {
                code_gen::FunctionCodeGenerator funcGenerator;
                funcGenerator.generateFunction(function);

                {
                    std::lock_guard<std::mutex> lock{mtx};
                    asmCode[function->getFunctionName()] = funcGenerator.getContext().asmCode;
                }

                doneLatch.count_down();
            }
        );
    }

    doneLatch.wait();

    writeCode(program);
}

void code_gen::CodeGenerator::writeCode(const ir::IRProgram* program){
    std::ofstream file{ outputPath };
    if(!file.is_open()){
        return;
    }

    // start of asm code
    file << code_gen::assembly::genStart();

    for(const auto& function : program->getFunctions()){
        for(const auto& instruction : asmCode[function->getFunctionName()]){
            file << instruction;
        }
    }

    file.close();
}

bool code_gen::CodeGenerator::successful() const noexcept {
    return std::filesystem::exists(outputPath);
}