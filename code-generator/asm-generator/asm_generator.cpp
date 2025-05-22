#include "asm_generator.hpp"
#include <format>

AsmGenerator::AsmGenerator(std::string& output) : output{output} {}

AsmGenerator::~AsmGenerator(){
    if(output.is_open()){
        output.close();
    }
}

bool AsmGenerator::isOpen(){
    return output.is_open();
}

void AsmGenerator::genStart(){
    output << ".global _start\n";
    output << ".text\n\n";
    output << "_start:\n";
    output << "\tjmp main\n";
}

void AsmGenerator::genMov(std::string l, std::string r, std::string ext){
    output << std::format("\tmov{} {}, {}\n", ext, l ,r);
}

void AsmGenerator::genCmp(std::string l, std::string r){
    output << std::format("\tcmp {}, {}\n", l, r);
}

void AsmGenerator::genOperation(std::string op, std::string l, std::string r){
    if(r != ""){
        output << std::format("\t{} {}, {}\n", op, l, r);
    }
    else{
        output << std::format("\t{} {}\n", op, l);
    }
}

void AsmGenerator::genLabel(std::string label){
    output << std::format("{}:\n", label);
}

void AsmGenerator::genRet(){
    output << "\tret\n";
}

void AsmGenerator::genJmp(std::string jmp, std::string label){
    output << std::format("\t{} {}\n", jmp, label);
}

void AsmGenerator::genCall(std::string func){
    output << std::format("\tcall {}\n", func);
}

void AsmGenerator::genPush(std::string r){
    output << std::format("\tpush {}\n", r);
}

void AsmGenerator::genPop(std::string r){
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
