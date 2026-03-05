#include "../ASTFunctionCallSt.hpp"

#include <memory>
#include <string>
#include <format>
#include <iostream>

ASTFunctionCallSt::ASTFunctionCallSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

void ASTFunctionCallSt::initFunctionCallSt(std::unique_ptr<ASTFunctionCall> funcCall) {
    functionCall = std::move(funcCall);
}

void ASTFunctionCallSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    functionCall->print(offset + 1);
}