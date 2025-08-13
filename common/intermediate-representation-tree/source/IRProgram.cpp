#include "../IRProgram.hpp"

#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <format>

IRProgram::IRProgram(IRNodeType ntype) : IRNode(ntype) {}

const std::vector<std::unique_ptr<IRFunction>>& IRProgram::getFunctions() const noexcept {
    return functions;
}

const IRFunction* IRProgram::getFunctionAtN(size_t n) const noexcept {
    return functions[n].get();
}

void IRProgram::resizeFunctions(size_t n){
    functions.resize(n);
}

void IRProgram::setFunctionAtN(std::unique_ptr<IRFunction> _function, size_t n){
    functions[n] = std::move(_function);
}

size_t IRProgram::getFunctionCount() const noexcept {
    return functions.size();
}

void IRProgram::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    for(const auto& function : functions){
        function->print(offset + 1);
    }
}