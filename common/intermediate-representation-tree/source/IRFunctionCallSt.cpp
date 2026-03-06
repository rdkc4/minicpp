#include "../IRFunctionCallSt.hpp"

IRFunctionCallSt::IRFunctionCallSt(IRNodeType ntype) : IRStatement{ ntype } {}

const IRFunctionCall* IRFunctionCallSt::getFunctionCall() const noexcept {
    return functionCall.get();
}

void IRFunctionCallSt::setFunctionCallSt(std::unique_ptr<IRFunctionCall> call){
    functionCall = std::move(call);
}