#include "asm_generator.hpp"
#include <format>

AsmGenerator::AsmGenerator(std::string& file) : output{ file } {}

AsmGenerator::~AsmGenerator() noexcept = default;

bool AsmGenerator::isOpen() const {
    return output.is_open();
}

void AsmGenerator::genStart(){
    output << ".global _start\n";
    output << ".text\n\n";
    output << "_start:\n";
    output << "\tjmp main\n";
}

void AsmGenerator::genMov(std::string_view l, std::string_view r, std::string_view ext){
    output << std::format("\tmov{} {}, {}\n", ext, l ,r);
}

void AsmGenerator::genCmp(std::string_view l, std::string_view r){
    output << std::format("\tcmp {}, {}\n", l, r);
}

void AsmGenerator::genOperation(std::string_view op, std::string_view l, std::string_view r){
    if(r != ""){
        output << std::format("\t{} {}, {}\n", op, l, r);
    }
    else{
        output << std::format("\t{} {}\n", op, l);
    }
}

void AsmGenerator::genLabel(std::string_view label){
    output << std::format("{}:\n", label);
}

void AsmGenerator::genRet(){
    output << "\tret\n";
}

void AsmGenerator::genJmp(std::string_view jmp, std::string_view label){
    output << std::format("\t{} {}\n", jmp, label);
}

void AsmGenerator::genCall(std::string_view func){
    output << std::format("\tcall {}\n", func);
}

void AsmGenerator::genPush(std::string_view r){
    output << std::format("\tpush {}\n", r);
}

void AsmGenerator::genPop(std::string_view r){
    output << std::format("\tpop {}\n", r);
}

void AsmGenerator::genFuncPrologue(){
    genPush("%rbp");
    genMov("%rsp", "%rbp");
}

void AsmGenerator::genFuncEpilogue(){
    genMov("%rbp", "%rsp");
    genPop("%rbp");
}

void AsmGenerator::genExit(){
    output << "\tmov %rax, %rdi\n"; // return value to rdi
    output << "\tmovq $60, %rax\n";
    output << "\tsyscall\n";
}

void AsmGenerator::genNewLine(){
    output << "\n";
}
