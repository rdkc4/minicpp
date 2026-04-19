#include "../function_code_generator.hpp"

#include <memory>
#include <format>

#include "../../asm-generator/asm_instruction_generator.hpp"

code_gen::FunctionCodeGenerator::FunctionCodeGenerator() 
    : stmtGenerator{ ctx } {}

void code_gen::FunctionCodeGenerator::generateFunction(const ir::IRFunction* function){
    if(function->isPredefined()){
        return;
    }

    ctx.functionName = function->getFunctionName();

    // function label
    code_gen::assembly::genLabel(ctx.asmCode, ctx.functionName);
    code_gen::assembly::genFuncPrologue(ctx.asmCode);
    
    // allocation of local variables
    if(function->getRequiredMemory() != "0"){
        code_gen::assembly::genOperation(
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
    code_gen::assembly::genLabel(
        ctx.asmCode, 
        std::format("_{}_end", ctx.functionName)
    );
    
    // free local variables 
    if(function->getRequiredMemory() != "0"){
        code_gen::assembly::genOperation(
            ctx.asmCode, 
            "add", 
            std::format("${}", function->getRequiredMemory()), 
            "%rsp"
        );
    }

    code_gen::assembly::genFuncEpilogue(ctx.asmCode);

    if(function->getFunctionName() != "main"){
        code_gen::assembly::genRet(ctx.asmCode);
    }
    else{
        code_gen::assembly::genExit(ctx.asmCode);
    }

}

void code_gen::FunctionCodeGenerator::generateParameters(const ir::IRFunction* function){
    size_t i{ 2 };
    for(const auto& parameter : function->getParameters()){
        // mapping parameter to address relative to %rbp (+n(%rbp))
        ctx.variableMap.insert({
            parameter->getParameterName(), 
            std::format("{}(%rbp)", i * code_gen::assembly::regSize)
        });
        ++i;
    }
}

const code_gen::CodeGeneratorFunctionContext& 
code_gen::FunctionCodeGenerator::getContext() const noexcept {
    return ctx;
}