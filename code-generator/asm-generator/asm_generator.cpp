#include "asm_generator.hpp"
#include <format>

AsmGenerator::AsmGenerator() = default;

const std::string AsmGenerator::genStart(){
    return std::format("{}{}{}{}", ".global _start\n", ".text\n\n", "_start:\n", "\tjmp main\n");
}

void AsmGenerator::genMov(std::vector<std::string>& asmCode, std::string_view l, std::string_view r, std::string_view ext){
    asmCode.push_back(std::format("\tmov{} {}, {}\n", ext, l ,r));
}

void AsmGenerator::genCmp(std::vector<std::string>& asmCode, std::string_view l, std::string_view r){
    asmCode.push_back(std::format("\tcmp {}, {}\n", l, r));
}

void AsmGenerator::genOperation(std::vector<std::string>& asmCode, std::string_view op, std::string_view l, std::string_view r){
    if(r != ""){
        asmCode.push_back(std::format("\t{} {}, {}\n", op, l, r));
    }
    else{
        asmCode.push_back(std::format("\t{} {}\n", op, l));
    }
}

void AsmGenerator::genLabel(std::vector<std::string>& asmCode, std::string_view label){
    asmCode.push_back(std::format("{}:\n", label));
}

void AsmGenerator::genRet(std::vector<std::string>& asmCode){
    asmCode.push_back("\tret\n");
}

void AsmGenerator::genJmp(std::vector<std::string>& asmCode, std::string_view jmp, std::string_view label){
    asmCode.push_back(std::format("\t{} {}\n", jmp, label));
}

void AsmGenerator::genCall(std::vector<std::string>& asmCode, std::string_view func){
    asmCode.push_back(std::format("\tcall {}\n", func));
}

void AsmGenerator::genPush(std::vector<std::string>& asmCode, std::string_view r){
    asmCode.push_back(std::format("\tpush {}\n", r));
}

void AsmGenerator::genPop(std::vector<std::string>& asmCode, std::string_view r){
    asmCode.push_back(std::format("\tpop {}\n", r));
}

void AsmGenerator::genFuncPrologue(std::vector<std::string>& asmCode){
    genPush(asmCode, "%rbp");
    genMov(asmCode, "%rsp", "%rbp");
}

void AsmGenerator::genFuncEpilogue(std::vector<std::string>& asmCode){
    genMov(asmCode, "%rbp", "%rsp");
    genPop(asmCode, "%rbp");
}

void AsmGenerator::genExit(std::vector<std::string>& asmCode){
    asmCode.push_back(std::format("{}{}{}", "\tmov %rax, %rdi\n", "\tmovq $60, %rax\n", "\tsyscall\n")); // return value in %rdi
}

void AsmGenerator::genNewLine(std::vector<std::string>& asmCode){
    asmCode.push_back("\n");
}

void AsmGenerator::printfFunction(std::vector<std::string>& printfFunc){
    genLabel(printfFunc, "_printf");
    genFuncPrologue(printfFunc);
    genOperation(printfFunc, "sub", "$32", "%rsp");
    genOperation(printfFunc, "lea", "31(%rsp), %rsi");

    genMov(printfFunc, "%rax", "%r9");
    genMov(printfFunc, "$10", "%rcx");
    genMov(printfFunc, "$0", "%rbx");

    genOperation(printfFunc, "cmp", "$0", "%r9");
    genJmp(printfFunc, "jne", "_printf_loop");

    genMov(printfFunc, "$'0'", "-1(%rsi)", "b");
    genOperation(printfFunc, "dec", "%rsi");
    genOperation(printfFunc, "inc", "%rbx");
    genJmp(printfFunc, "jmp", "_printf_done");

    genLabel(printfFunc, "_printf_loop");
    genOperation(printfFunc, "xor", "%rdx", "%rdx");
    genMov(printfFunc, "%r9", "%rax");
    genOperation(printfFunc, "div", "%rcx");
    genOperation(printfFunc, "add", "$'0'", std::format("{}{}", "%", "dl"));
    genOperation(printfFunc, "dec", "%rsi");
    genMov(printfFunc, std::format("{}{}", "%", "dl"), "(%rsi)");
    genOperation(printfFunc, "inc", "%rbx");
    genMov(printfFunc, "%rax", "%r9");
    genOperation(printfFunc, "test", "%rax", "%rax");
    genJmp(printfFunc, "jnz", "_printf_loop");

    genLabel(printfFunc, "_printf_done");
    genOperation(printfFunc, "dec", "%rsi");
    genMov(printfFunc,"$'\\n'", "(%rsi)", "b");
    genOperation(printfFunc, "inc", "%rbx");

    genMov(printfFunc, "$1", "%rax");
    genMov(printfFunc, "$1", "%rdi");
    genMov(printfFunc, "%rbx", "%rdx");
    printfFunc.push_back("\tsyscall\n");
    
    genFuncEpilogue(printfFunc);
    genRet(printfFunc);
}