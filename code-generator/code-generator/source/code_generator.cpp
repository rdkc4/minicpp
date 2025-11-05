#include "../code_generator.hpp"

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "../../../thread-pool/thread_pool.hpp"

#include "../../asm-generator/asm_instruction_generator.hpp"
#include "../../asm-generator/asm_function_generator.hpp"

CodeGenerator::CodeGenerator(const std::string& filePath) : outputPath{ filePath }, funcGenerator{ asmCode } {}

std::atomic<size_t> CodeGenerator::labelNum{ 0 };

size_t CodeGenerator::getNextLabelNum() noexcept {
    return labelNum.fetch_add(1);
}

void CodeGenerator::generateCode(const IRProgram* _program){
    funcGenerator.initFunctions(_program);

    ThreadPool threadPool{ std::thread::hardware_concurrency() };

    // counter of generated functions
    std::atomic<size_t> done{0};
    std::condition_variable doneCv;
    std::mutex doneMtx;

    const size_t total = _program->getFunctionCount();

    for(const auto& _function : _program->getFunctions()){
        threadPool.enqueue([&, _function=_function.get()]{
            funcGenerator.generateFunction(_function);

            if(done.fetch_add(1) + 1 == total){
                doneCv.notify_one();
            }
        });
    }

    {
        // wait until each function is generated
        std::unique_lock<std::mutex> lock(doneMtx);
        doneCv.wait(lock, [&] { return done == total; });
    }

    writeCode(_program);
}

void CodeGenerator::writeCode(const IRProgram* _program){
    std::ofstream file{ outputPath };
    if(!file.is_open()){
        return;
    }

    // start of asm code
    file << AsmGenerator::Instruction::genStart();

    for(const auto& _function : _program->getFunctions()){
        for(const auto& instruction : asmCode[_function->getFunctionName()]){
            file << instruction;
        }
    }
    
    if(funcGenerator.hasPrint()){
        std::vector<std::string> printfFunc;
        AsmGenerator::Function::printfFunction(printfFunc);
        for(const auto& instruction : printfFunc){
            file << instruction;
        }
    }
    file.close();
}

bool CodeGenerator::successful() const noexcept {
    return std::filesystem::exists(outputPath);
}