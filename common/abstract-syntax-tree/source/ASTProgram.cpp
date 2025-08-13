#include "../ASTProgram.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <format>

ASTProgram::ASTProgram(const Token& token, ASTNodeType ntype) : ASTNode(token, ntype) {}

const std::vector<std::unique_ptr<ASTFunction>>& ASTProgram::getFunctions() const noexcept {
    return functions;
}

size_t ASTProgram::getFunctionCount() const noexcept {
    return functions.size();
}

const ASTFunction* ASTProgram::getFunctionAtN(size_t n) const noexcept {
    return functions[n].get();
}

void ASTProgram::addFunction(std::unique_ptr<ASTFunction> function){
    functions.push_back(std::move(function));
}

void ASTProgram::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    for(const auto& func : functions){
        func->print(offset + 1);
    }
}