#include "code_generator.hpp"
#include <format>

void CodeGenerator::genStart(){
    generatedCode << ".global _start\n";
    generatedCode << ".text\n\n";
    generatedCode << "_start:\n";
    generatedCode << "\tjmp main\n";
}

void CodeGenerator::genMov(std::string l, std::string r, std::string ext){
    generatedCode << std::format("\tmov{} {}, {}\n", ext, l ,r);
}

void CodeGenerator::genCmp(std::string l, std::string r){
    generatedCode << std::format("\tcmp {}, {}\n", l, r);
}

void CodeGenerator::genPush(std::string r){
    generatedCode << std::format("\tpush {}\n", r);
}

void CodeGenerator::genPop(std::string r){
    generatedCode << std::format("\tpop {}\n", r);
}

void CodeGenerator::genOperation(std::string op, std::string l, std::string r){
    if(r != ""){
        generatedCode << std::format("\t{} {}, {}\n", op, l, r);
    }
    else{
        generatedCode << std::format("\t{} {}\n", op, l);
    }
}

void CodeGenerator::genLabel(std::string label){
    generatedCode << std::format("{}:\n", label);
}

void CodeGenerator::genRet(){
    generatedCode << "\tret\n";
}

void CodeGenerator::genExit(){
    generatedCode << "\tmov %rax, %rdi\n"; // return value to rdi
    generatedCode << "\tmovq $60, %rax\n";
    generatedCode << "\tsyscall\n";
}

void CodeGenerator::genJmp(std::string jmp, std::string label){
    generatedCode << std::format("\t{} {}\n", jmp, label);
}

void CodeGenerator::genCall(std::string func){
    generatedCode << std::format("\tcall {}\n", func);
}

void CodeGenerator::genNewLine(){
    generatedCode << "\n";
}
