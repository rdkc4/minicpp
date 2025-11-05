#include "../asm_instruction_generator.hpp"

#include <format>

// start of an asm file
const std::string AsmGenerator::Instruction::genStart(){
    return std::format(
        "{}{}{}{}", 
        ".global _start\n", 
        ".text\n\n", 
        "_start:\n", 
        "\tjmp main\n"
    );
}

void AsmGenerator::Instruction::genMov(std::vector<std::string>& asmCode, std::string_view l, std::string_view r, std::string_view ext){
    asmCode.push_back(std::format("\tmov{} {}, {}\n", ext, l ,r));
}

void AsmGenerator::Instruction::genCmp(std::vector<std::string>& asmCode, std::string_view l, std::string_view r){
    asmCode.push_back(std::format("\tcmp {}, {}\n", l, r));
}

void AsmGenerator::Instruction::genOperation(std::vector<std::string>& asmCode, std::string_view op, std::string_view l, std::string_view r){
    if(r != ""){
        asmCode.push_back(std::format("\t{} {}, {}\n", op, l, r));
    }
    else{
        asmCode.push_back(std::format("\t{} {}\n", op, l));
    }
}

void AsmGenerator::Instruction::genLabel(std::vector<std::string>& asmCode, std::string_view label){
    asmCode.push_back(std::format("{}:\n", label));
}

void AsmGenerator::Instruction::genRet(std::vector<std::string>& asmCode){
    asmCode.push_back("\tret\n");
}

void AsmGenerator::Instruction::genJmp(std::vector<std::string>& asmCode, std::string_view jmp, std::string_view label){
    asmCode.push_back(std::format("\t{} {}\n", jmp, label));
}

void AsmGenerator::Instruction::genCall(std::vector<std::string>& asmCode, std::string_view func){
    asmCode.push_back(std::format("\tcall {}\n", func));
}

void AsmGenerator::Instruction::genPush(std::vector<std::string>& asmCode, std::string_view r){
    asmCode.push_back(std::format("\tpush {}\n", r));
}

void AsmGenerator::Instruction::genPop(std::vector<std::string>& asmCode, std::string_view r){
    asmCode.push_back(std::format("\tpop {}\n", r));
}

void AsmGenerator::Instruction::genFuncPrologue(std::vector<std::string>& asmCode){
    genPush(asmCode, "%rbp");
    genMov(asmCode, "%rsp", "%rbp");
}

void AsmGenerator::Instruction::genFuncEpilogue(std::vector<std::string>& asmCode){
    genMov(asmCode, "%rbp", "%rsp");
    genPop(asmCode, "%rbp");
}

void AsmGenerator::Instruction::genExit(std::vector<std::string>& asmCode){
    asmCode.push_back(std::format("{}{}{}", "\tmov %rax, %rdi\n", "\tmovq $60, %rax\n", "\tsyscall\n")); // return value in %rdi
}

void AsmGenerator::Instruction::genNewLine(std::vector<std::string>& asmCode){
    asmCode.push_back("\n");
}