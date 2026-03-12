#include "../function_code_generator.hpp"

#include <memory>
#include <format>

#include "../../asm-generator/asm_instruction_generator.hpp"

FunctionCodeGenerator::FunctionCodeGenerator(std::unordered_map<std::string, std::vector<std::string>>& asmCode) : asmCode{ asmCode } {}

thread_local CodeGeneratorThreadContext FunctionCodeGenerator::codeGenContext;

CodeGeneratorThreadContext& FunctionCodeGenerator::getContext() noexcept {
    return codeGenContext;
}

void FunctionCodeGenerator::initFunctions(const IRProgram* program){
    for(const auto& function : program->getFunctions()){
        asmCode[function->getFunctionName()] = {};
    }
}

void FunctionCodeGenerator::generateFunction(const IRFunction* function){
    if(function->isPredefined()){
        return;
    }

    codeGenContext.init(function->getFunctionName());

    // function label
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, codeGenContext.functionName);

    AsmGenerator::Instruction::genFuncPrologue(codeGenContext.asmCode);
    
    // allocation of local variables
    if(function->getRequiredMemory() != "0"){
        AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, "sub", std::format("${}", function->getRequiredMemory()), "%rsp");
    }

    generateParameters(function);

    for(const auto& stmt : function->getBody()){
        stmtGenerator.generateStmt(stmt.get());
    }

    // function end label
    AsmGenerator::Instruction::genLabel(codeGenContext.asmCode, std::format("{}_end", codeGenContext.functionName));
    
    // free local variables 
    if(function->getRequiredMemory() != "0"){
        AsmGenerator::Instruction::genOperation(codeGenContext.asmCode, "add", std::format("${}", function->getRequiredMemory()), "%rsp");
    }

    AsmGenerator::Instruction::genFuncEpilogue(codeGenContext.asmCode);

    if(function->getFunctionName() != "main"){
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

void FunctionCodeGenerator::generateParameters(const IRFunction* function){
    size_t i{ 2 };
    for(const auto& parameter : function->getParameters()){
        // mapping parameter to address relative to %rbp (+n(%rbp))
        codeGenContext.variableMap.insert({parameter->getParameterName(), std::format("{}(%rbp)", i * AsmGenerator::Instruction::regSize)});
        ++i;
    }
}