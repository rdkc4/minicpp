#include "../function_code_generator.hpp"

#include <memory>
#include <format>

#include "../../asm-generator/asm_instruction_generator.hpp"

FunctionCodeGenerator::FunctionCodeGenerator() 
    : stmtGenerator{ ctx } {}

void FunctionCodeGenerator::generateFunction(const IR::node::IRFunction* function){
    if(function->isPredefined()){
        return;
    }

    ctx.functionName = function->getFunctionName();

    // function label
    AsmGenerator::Instruction::genLabel(ctx.asmCode, ctx.functionName);
    AsmGenerator::Instruction::genFuncPrologue(ctx.asmCode);
    
    // allocation of local variables
    if(function->getRequiredMemory() != "0"){
        AsmGenerator::Instruction::genOperation(
            ctx.asmCode, 
            "sub", 
            std::format("${}", function->getRequiredMemory()), 
            "%rsp"
        );
    }

    generateParameters(function);

    for(const auto& stmt : function->getBody()){
        stmtGenerator.generateStmt(stmt.get());
    }

    // function end label
    AsmGenerator::Instruction::genLabel(
        ctx.asmCode, 
        std::format("_{}_end", ctx.functionName)
    );
    
    // free local variables 
    if(function->getRequiredMemory() != "0"){
        AsmGenerator::Instruction::genOperation(
            ctx.asmCode, 
            "add", 
            std::format("${}", function->getRequiredMemory()), 
            "%rsp"
        );
    }

    AsmGenerator::Instruction::genFuncEpilogue(ctx.asmCode);

    if(function->getFunctionName() != "main"){
        AsmGenerator::Instruction::genRet(ctx.asmCode);
    }
    else{
        AsmGenerator::Instruction::genExit(ctx.asmCode);
    }

}

void FunctionCodeGenerator::generateParameters(const IR::node::IRFunction* function){
    size_t i{ 2 };
    for(const auto& parameter : function->getParameters()){
        // mapping parameter to address relative to %rbp (+n(%rbp))
        ctx.variableMap.insert({
            parameter->getParameterName(), 
            std::format("{}(%rbp)", i * AsmGenerator::Instruction::regSize)
        });
        ++i;
    }
}

const CodeGeneratorFunctionContext& FunctionCodeGenerator::getContext() const noexcept {
    return ctx;
}