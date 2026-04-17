#include "../asm_instruction_generator.hpp"

#include <format>

std::atomic<size_t> assembly::labelNum{0};

size_t assembly::getNextLabelNum() noexcept {
    return labelNum.fetch_add(1, std::memory_order_relaxed);
}

// start of an asm file
const std::string assembly::genStart(){
    return std::format(
        "{}{}{}{}", 
        ".global _start\n", 
        ".text\n\n", 
        "_start:\n", 
        "\tjmp main\n"
    );
}

void assembly::genMov(
    std::vector<std::string>& asmCode, 
    std::string_view src, 
    std::string_view dest, 
    std::string_view ext
){
    asmCode.push_back(std::format("\tmov{} {}, {}\n", ext, src, dest));
}

void assembly::genSet(
    std::vector<std::string>& asmCode, 
    std::string_view dest, 
    std::string_view ext
){
    asmCode.push_back(std::format("\tset{} {}\n", ext, dest));
}

void assembly::genSetcc(
    std::vector<std::string>& asmCode, 
    std::string_view setcc, 
    std::string_view dest
){
    asmCode.push_back(std::format("\t{} {}\n", setcc, dest));
}

void assembly::genTest(
    std::vector<std::string>& asmCode, 
    std::string_view op, 
    std::string_view ext
){
    asmCode.push_back(std::format("\ttest{} {}, {}\n", ext, op, op));
}

void assembly::genTest(
    std::vector<std::string> &asmCode, 
    std::string_view lOp, 
    std::string_view rOp, 
    std::string_view ext
){
    asmCode.push_back(std::format("\ttest{} {}, {}\n", ext, lOp, rOp));
}

void assembly::genCmp(
    std::vector<std::string>& asmCode, 
    std::string_view lOp, 
    std::string_view rOp
){
    asmCode.push_back(std::format("\tcmp {}, {}\n", lOp, rOp));
}

void assembly::genOperation(
    std::vector<std::string>& asmCode, 
    std::string_view operation, 
    std::string_view src, 
    std::string_view dest
){
    asmCode.push_back(std::format("\t{} {}, {}\n", operation, src, dest));
}

void assembly::genOperation(
    std::vector<std::string>& asmCode, 
    std::string_view operation, 
    std::string_view dest
){
    asmCode.push_back(std::format("\t{} {}\n", operation, dest));
}

void assembly::genLabel(
    std::vector<std::string>& asmCode, 
    std::string_view label
){
    asmCode.push_back(std::format("{}:\n", label));
}

void assembly::genRet(std::vector<std::string>& asmCode){
    asmCode.push_back("\tret\n");
}

void assembly::genJmp(
    std::vector<std::string>& asmCode, 
    std::string_view label
){
    asmCode.push_back(std::format("\tjmp {}\n", label));
}

void assembly::genJcc(
    std::vector<std::string>& asmCode, 
    std::string_view jcc, 
    std::string_view label
){
    asmCode.push_back(std::format("\t{} {}\n", jcc, label));
}

void assembly::genCall(
    std::vector<std::string>& asmCode, 
    std::string_view func
){
    asmCode.push_back(std::format("\tcall {}\n", func));
}

void assembly::genPush(
    std::vector<std::string>& asmCode, 
    std::string_view src
){
    asmCode.push_back(std::format("\tpush {}\n", src));
}

void assembly::genPop(
    std::vector<std::string>& asmCode, 
    std::string_view dest
){
    asmCode.push_back(std::format("\tpop {}\n", dest));
}

void assembly::genFuncPrologue(std::vector<std::string>& asmCode){
    genPush(asmCode, "%rbp");
    genMov(asmCode, "%rsp", "%rbp");
}

void assembly::genFuncEpilogue(std::vector<std::string>& asmCode){
    genMov(asmCode, "%rbp", "%rsp");
    genPop(asmCode, "%rbp");
}

void assembly::genExit(std::vector<std::string>& asmCode){
    asmCode.push_back(std::format(
        "{}{}{}", 
        "\tmov %rax, %rdi\n", 
        "\tmovq $60, %rax\n", 
        "\tsyscall\n")
    ); // return value in %rdi
}

void assembly::genNewLine(std::vector<std::string>& asmCode){
    asmCode.push_back("\n");
}