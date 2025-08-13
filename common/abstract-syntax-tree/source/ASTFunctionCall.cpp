#include "../ASTFunctionCall.hpp"

#include <memory>
#include <vector>
#include <string>
#include <format>
#include <iostream>

ASTFunctionCall::ASTFunctionCall(const Token& token, ASTNodeType ntype, Types type) : ASTExpression(token, ntype, type) {}

const std::vector<std::unique_ptr<ASTExpression>>& ASTFunctionCall::getArguments() const noexcept {
    return arguments;
}

const ASTExpression* ASTFunctionCall::getArgumentAtN(size_t n) const noexcept{
    return arguments[n].get();
}

void ASTFunctionCall::addArgument(std::unique_ptr<ASTExpression> arg){
    arguments.push_back(std::move(arg));
}

size_t ASTFunctionCall::getArgumentCount() const noexcept {
    return arguments.size();
}

void ASTFunctionCall::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    std::cout << std::format("{}|-> {}", std::string((offset + 1) * 2, ' '), "ARGUMENTS\n");
    for(const auto& arg : arguments){
        arg->print(offset + 2);
    }
}