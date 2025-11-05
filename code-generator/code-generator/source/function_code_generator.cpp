#include "../function_code_generator.hpp"

#include "../../asm-generator/asm_instruction_generator.hpp"

FunctionCodeGenerator::FunctionCodeGenerator(std::unordered_map<std::string, std::vector<std::string>>& asmCode) : asmCode{ asmCode } {}

thread_local CodeGeneratorThreadContext FunctionCodeGenerator::codeGenContext;

std::atomic<bool> FunctionCodeGenerator::prints{ false };

CodeGeneratorThreadContext& FunctionCodeGenerator::getContext() noexcept {
    return codeGenContext;
}

void FunctionCodeGenerator::updatePrints(bool _prints) noexcept {
    prints.store(_prints);
}

bool FunctionCodeGenerator::hasPrint() const noexcept {
    return prints.load();
}

void FunctionCodeGenerator::initFunctions(const IRProgram* _program){
    for(const auto& _function : _program->getFunctions()){
        asmCode[_function->getFunctionName()] = {};
    }
}

void FunctionCodeGenerator::generateFunction(const IRFunction* _function){
    codeGenContext.init(_function->getFunctionName());

    // function label
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, codeGenContext.functionName);

    AsmGenerator::Instruction::genFuncPrologue(codeGenContext.asmCode);
    
    // allocation of local variables
    if(_function->getRequiredMemory() != "0"){
        AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, "sub", std::format("${}", _function->getRequiredMemory()), "%rsp");
    }

    generateParameter(_function->getParameters());

    for(const auto& _statement : _function->getBody()){
        stmtGenerator.generateStatement(_statement.get());
    }

    // function end label
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("{}_end", codeGenContext.functionName));
    
    // free local variables 
    if(_function->getRequiredMemory() != "0"){
        AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, "add", std::format("${}", _function->getRequiredMemory()), "%rsp");
    }

    AsmGenerator::Instruction::genFuncEpilogue(codeGenContext.asmCode);

    if(_function->getFunctionName() != "main"){
        AsmGenerator::Instruction::genRet(codeGenContext.asmCode);
    }
    else{
        AsmGenerator::Instruction::genExit(codeGenContext.asmCode);
    }

    {
        std::lock_guard<std::mutex> lock(asmMtx);
        asmCode[codeGenContext.functionName] = std::move(codeGenContext.asmCode);
    }

    codeGenContext.reset();
}

void FunctionCodeGenerator::generateParameter(const std::vector<std::unique_ptr<IRParameter>>& _parameters){
    size_t i{ 2 };
    for(const auto& _parameter : _parameters){
        // mapping parameter to address relative to %rbp (+n(%rbp))
        codeGenContext.variableMap.insert({_parameter->getParName(), std::format("{}(%rbp)", i * AsmGenerator::Instruction::regSize)});
        ++i;
    }
}