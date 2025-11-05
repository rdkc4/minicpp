#include "../asm_function_generator.hpp"
#include "../asm_instruction_generator.hpp"

#include <format>

void AsmGenerator::Function::printfFunction(std::vector<std::string>& printfFunc){
    AsmGenerator::Instruction::genLabel(printfFunc, "_printf");
    AsmGenerator::Instruction::genFuncPrologue(printfFunc);
    AsmGenerator::Instruction::genOperation(printfFunc, "sub", "$32", "%rsp");
    AsmGenerator::Instruction::genOperation(printfFunc, "lea", "31(%rsp), %rsi");

    AsmGenerator::Instruction::genMov(printfFunc, "%rax", "%r9");
    AsmGenerator::Instruction::genMov(printfFunc, "$10", "%rcx");
    AsmGenerator::Instruction::genMov(printfFunc, "$0", "%rbx");

    AsmGenerator::Instruction::genOperation(printfFunc, "cmp", "$0", "%r9");
    AsmGenerator::Instruction::genJmp(printfFunc, "jne", "_printf_loop");

    AsmGenerator::Instruction::genMov(printfFunc, "$'0'", "-1(%rsi)", "b");
    AsmGenerator::Instruction::genOperation(printfFunc, "dec", "%rsi");
    AsmGenerator::Instruction::genOperation(printfFunc, "inc", "%rbx");
    AsmGenerator::Instruction::genJmp(printfFunc, "jmp", "_printf_done");

    AsmGenerator::Instruction::genLabel(printfFunc, "_printf_loop");
    AsmGenerator::Instruction::genOperation(printfFunc, "xor", "%rdx", "%rdx");
    AsmGenerator::Instruction::genMov(printfFunc, "%r9", "%rax");
    AsmGenerator::Instruction::genOperation(printfFunc, "div", "%rcx");
    AsmGenerator::Instruction::genOperation(printfFunc, "add", "$'0'", std::format("{}{}", "%", "dl"));
    AsmGenerator::Instruction::genOperation(printfFunc, "dec", "%rsi");
    AsmGenerator::Instruction::genMov(printfFunc, std::format("{}{}", "%", "dl"), "(%rsi)");
    AsmGenerator::Instruction::genOperation(printfFunc, "inc", "%rbx");
    AsmGenerator::Instruction::genMov(printfFunc, "%rax", "%r9");
    AsmGenerator::Instruction::genOperation(printfFunc, "test", "%rax", "%rax");
    AsmGenerator::Instruction::genJmp(printfFunc, "jnz", "_printf_loop");

    AsmGenerator::Instruction::genLabel(printfFunc, "_printf_done");
    AsmGenerator::Instruction::genOperation(printfFunc, "dec", "%rsi");
    AsmGenerator::Instruction::genMov(printfFunc,"$'\\n'", "(%rsi)", "b");
    AsmGenerator::Instruction::genOperation(printfFunc, "inc", "%rbx");

    AsmGenerator::Instruction::genMov(printfFunc, "$1", "%rax");
    AsmGenerator::Instruction::genMov(printfFunc, "$1", "%rdi");
    AsmGenerator::Instruction::genMov(printfFunc, "%rbx", "%rdx");
    printfFunc.push_back("\tsyscall\n");
    
    AsmGenerator::Instruction::genFuncEpilogue(printfFunc);
    AsmGenerator::Instruction::genRet(printfFunc);
}