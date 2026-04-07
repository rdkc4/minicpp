#include "../code_generator.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <latch>

#include "../../asm-generator/asm_instruction_generator.hpp"

CodeGenerator::CodeGenerator(const std::string& filePath, ThreadPool& threadPool) 
    : threadPool{ threadPool}, outputPath{ filePath }, funcGenerator{ asmCode } {}

std::atomic<size_t> CodeGenerator::labelNum{ 0 };

size_t CodeGenerator::getNextLabelNum() noexcept {
    return labelNum.fetch_add(1);
}

void CodeGenerator::generateProgram(const IRProgram* program){
    funcGenerator.initFunctions(program);

    std::latch doneLatch{ static_cast<ptrdiff_t>(program->getFunctionCount()) };

    for(const auto& function : program->getFunctions()){
        threadPool.enqueue(
            [this, function=function.get(), &doneLatch] -> void {
                funcGenerator.generateFunction(function);
                doneLatch.count_down();
            }
        );
    }

    doneLatch.wait();

    writeCode(program);
}

void CodeGenerator::writeCode(const IRProgram* program){
    std::ofstream file{ outputPath };
    if(!file.is_open()){
        return;
    }

    // start of asm code
    file << AsmGenerator::Instruction::genStart();

    for(const auto& function : program->getFunctions()){
        for(const auto& instruction : asmCode[function->getFunctionName()]){
            file << instruction;
        }
    }

    file.close();
}

bool CodeGenerator::successful() const noexcept {
    return std::filesystem::exists(outputPath);
}