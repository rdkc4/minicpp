#include "asm_instruction_generator.hpp"

#include <format>

std::atomic<size_t> code_gen::assembly::labelNum{0};

size_t code_gen::assembly::getNextLabelNum() noexcept {
    return labelNum.fetch_add(1, std::memory_order_relaxed);
}

// start of an asm file
const std::string code_gen::assembly::genStart(){
    return std::format(
        "{}{}{}{}", 
        ".global _start\n", 
        ".text\n\n", 
        "_start:\n", 
        "\tjmp main\n"
    );
}

void code_gen::assembly::genMov(
    std::vector<std::string>& asmCode, 
    std::string_view src, 
    std::string_view dest, 
    std::string_view ext
){
    asmCode.push_back(std::format("\tmov{} {}, {}\n", ext, src, dest));
}

void code_gen::assembly::genSet(
    std::vector<std::string>& asmCode, 
    std::string_view dest, 
    std::string_view ext
){
    asmCode.push_back(std::format("\tset{} {}\n", ext, dest));
}

void code_gen::assembly::genSetcc(
    std::vector<std::string>& asmCode, 
    std::string_view setcc, 
    std::string_view dest
){
    asmCode.push_back(std::format("\t{} {}\n", setcc, dest));
}

void code_gen::assembly::genTest(
    std::vector<std::string>& asmCode, 
    std::string_view op, 
    std::string_view ext
){
    asmCode.push_back(std::format("\ttest{} {}, {}\n", ext, op, op));
}

void code_gen::assembly::genTest(
    std::vector<std::string> &asmCode, 
    std::string_view lOp, 
    std::string_view rOp, 
    std::string_view ext
){
    asmCode.push_back(std::format("\ttest{} {}, {}\n", ext, lOp, rOp));
}

void code_gen::assembly::genCmp(
    std::vector<std::string>& asmCode, 
    std::string_view lOp, 
    std::string_view rOp
){
    asmCode.push_back(std::format("\tcmp {}, {}\n", lOp, rOp));
}

void code_gen::assembly::genOperation(
    std::vector<std::string>& asmCode, 
    std::string_view operation, 
    std::string_view src, 
    std::string_view dest
){
    asmCode.push_back(std::format("\t{} {}, {}\n", operation, src, dest));
}

void code_gen::assembly::genOperation(
    std::vector<std::string>& asmCode, 
    std::string_view operation, 
    std::string_view dest
){
    asmCode.push_back(std::format("\t{} {}\n", operation, dest));
}

void code_gen::assembly::genLabel(
    std::vector<std::string>& asmCode, 
    std::string_view label
){
    asmCode.push_back(std::format("{}:\n", label));
}

void code_gen::assembly::genRet(std::vector<std::string>& asmCode){
    asmCode.push_back("\tret\n");
}

void code_gen::assembly::genJmp(
    std::vector<std::string>& asmCode, 
    std::string_view label
){
    asmCode.push_back(std::format("\tjmp {}\n", label));
}

void code_gen::assembly::genJcc(
    std::vector<std::string>& asmCode, 
    std::string_view jcc, 
    std::string_view label
){
    asmCode.push_back(std::format("\t{} {}\n", jcc, label));
}

void code_gen::assembly::genCall(
    std::vector<std::string>& asmCode, 
    std::string_view func
){
    asmCode.push_back(std::format("\tcall {}\n", func));
}

void code_gen::assembly::genPush(
    std::vector<std::string>& asmCode, 
    std::string_view src
){
    asmCode.push_back(std::format("\tpush {}\n", src));
}

void code_gen::assembly::genPop(
    std::vector<std::string>& asmCode, 
    std::string_view dest
){
    asmCode.push_back(std::format("\tpop {}\n", dest));
}

void code_gen::assembly::genFuncPrologue(std::vector<std::string>& asmCode){
    genPush(asmCode, "%rbp");
    genMov(asmCode, "%rsp", "%rbp");
}

void code_gen::assembly::genFuncEpilogue(std::vector<std::string>& asmCode){
    genMov(asmCode, "%rbp", "%rsp");
    genPop(asmCode, "%rbp");
}

void code_gen::assembly::genExit(std::vector<std::string>& asmCode){
    asmCode.push_back(std::format(
        "{}{}{}", 
        "\tmov %rax, %rdi\n", 
        "\tmovq $60, %rax\n", 
        "\tsyscall\n")
    ); // return value in %rdi
}

void code_gen::assembly::genNewLine(std::vector<std::string>& asmCode){
    asmCode.push_back("\n");
}