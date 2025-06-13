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
