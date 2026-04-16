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

void AsmGenerator::Instruction::genMov(
    std::vector<std::string>& asmCode, 
    std::string_view src, 
    std::string_view dest, 
    std::string_view ext
){
    asmCode.push_back(std::format("\tmov{} {}, {}\n", ext, src, dest));
}

void AsmGenerator::Instruction::genSet(
    std::vector<std::string>& asmCode, 
    std::string_view dest, 
    std::string_view ext
){
    asmCode.push_back(std::format("\tset{} {}\n", ext, dest));
}

void AsmGenerator::Instruction::genSetcc(
    std::vector<std::string>& asmCode, 
    std::string_view setcc, 
    std::string_view dest
){
    asmCode.push_back(std::format("\t{} {}\n", setcc, dest));
}

void AsmGenerator::Instruction::genTest(
    std::vector<std::string>& asmCode, 
    std::string_view op, 
    std::string_view ext
){
    asmCode.push_back(std::format("\ttest{} {}, {}\n", ext, op, op));
}

void AsmGenerator::Instruction::genTest(
    std::vector<std::string> &asmCode, 
    std::string_view lOp, 
    std::string_view rOp, 
    std::string_view ext
){
    asmCode.push_back(std::format("\ttest{} {}, {}\n", ext, lOp, rOp));
}

void AsmGenerator::Instruction::genCmp(
    std::vector<std::string>& asmCode, 
    std::string_view lOp, 
    std::string_view rOp
){
    asmCode.push_back(std::format("\tcmp {}, {}\n", lOp, rOp));
}

void AsmGenerator::Instruction::genOperation(
    std::vector<std::string>& asmCode, 
    std::string_view operation, 
    std::string_view src, 
    std::string_view dest
){
    asmCode.push_back(std::format("\t{} {}, {}\n", operation, src, dest));
}

void AsmGenerator::Instruction::genOperation(
    std::vector<std::string>& asmCode, 
    std::string_view operation, 
    std::string_view dest
){
    asmCode.push_back(std::format("\t{} {}\n", operation, dest));
}

void AsmGenerator::Instruction::genLabel(
    std::vector<std::string>& asmCode, 
    std::string_view label
){
    asmCode.push_back(std::format("{}:\n", label));
}

void AsmGenerator::Instruction::genRet(std::vector<std::string>& asmCode){
    asmCode.push_back("\tret\n");
}

void AsmGenerator::Instruction::genJmp(
    std::vector<std::string>& asmCode, 
    std::string_view label
){
    asmCode.push_back(std::format("\tjmp {}\n", label));
}

void AsmGenerator::Instruction::genJcc(
    std::vector<std::string>& asmCode, 
    std::string_view jcc, 
    std::string_view label
){
    asmCode.push_back(std::format("\t{} {}\n", jcc, label));
}

void AsmGenerator::Instruction::genCall(
    std::vector<std::string>& asmCode, 
    std::string_view func
){
    asmCode.push_back(std::format("\tcall {}\n", func));
}

void AsmGenerator::Instruction::genPush(
    std::vector<std::string>& asmCode, 
    std::string_view src
){
    asmCode.push_back(std::format("\tpush {}\n", src));
}

void AsmGenerator::Instruction::genPop(
    std::vector<std::string>& asmCode, 
    std::string_view dest
){
    asmCode.push_back(std::format("\tpop {}\n", dest));
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
    asmCode.push_back(std::format(
        "{}{}{}", 
        "\tmov %rax, %rdi\n", 
        "\tmovq $60, %rax\n", 
        "\tsyscall\n")
    ); // return value in %rdi
}

void AsmGenerator::Instruction::genNewLine(std::vector<std::string>& asmCode){
    asmCode.push_back("\n");
}