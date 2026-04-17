#include "../function_code_generator.hpp"

#include <memory>
#include <format>

#include "../../asm-generator/asm_instruction_generator.hpp"

FunctionCodeGenerator::FunctionCodeGenerator() 
    : stmtGenerator{ ctx } {}

void FunctionCodeGenerator::generateFunction(const ir::IRFunction* function){
    if(function->isPredefined()){
        return;
    }

    ctx.functionName = function->getFunctionName();

    // function label
    assembly::genLabel(ctx.asmCode, ctx.functionName);
    assembly::genFuncPrologue(ctx.asmCode);
    
    // allocation of local variables
    if(function->getRequiredMemory() != "0"){
        assembly::genOperation(
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
    assembly::genLabel(
        ctx.asmCode, 
        std::format("_{}_end", ctx.functionName)
    );
    
    // free local variables 
    if(function->getRequiredMemory() != "0"){
        assembly::genOperation(
            ctx.asmCode, 
            "add", 
            std::format("${}", function->getRequiredMemory()), 
            "%rsp"
        );
    }

    assembly::genFuncEpilogue(ctx.asmCode);

    if(function->getFunctionName() != "main"){
        assembly::genRet(ctx.asmCode);
    }
    else{
        assembly::genExit(ctx.asmCode);
    }

}

void FunctionCodeGenerator::generateParameters(const ir::IRFunction* function){
    size_t i{ 2 };
    for(const auto& parameter : function->getParameters()){
        // mapping parameter to address relative to %rbp (+n(%rbp))
        ctx.variableMap.insert({
            parameter->getParameterName(), 
            std::format("{}(%rbp)", i * assembly::regSize)
        });
        ++i;
    }
}

const CodeGeneratorFunctionContext& FunctionCodeGenerator::getContext() const noexcept {
    return ctx;
}