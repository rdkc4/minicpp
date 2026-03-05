#include "../IRFunctionCallSt.hpp"

#include <iostream>
#include <format>

IRFunctionCallSt::IRFunctionCallSt(IRNodeType ntype) : IRStatement{ ntype } {}

const IRFunctionCall* IRFunctionCallSt::getFunctionCall() const noexcept {
    return functionCall.get();
}

void IRFunctionCallSt::setFunctionCallSt(std::unique_ptr<IRFunctionCall> call){
    functionCall = std::move(call);
}

void IRFunctionCallSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    functionCall->print(offset + 1);
}