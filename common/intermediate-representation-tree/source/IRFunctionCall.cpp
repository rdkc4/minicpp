#include "../IRFunctionCall.hpp"

#include <vector>
#include <string>
#include <memory>
#include <format>
#include <iostream>

IRFunctionCall::IRFunctionCall(IRNodeType ntype, const std::string& callName, Types type) : IRExpression(ntype, type), callName{ callName } {}

const std::vector<std::unique_ptr<IRExpression>>& IRFunctionCall::getArguments() const noexcept {
    return arguments;
}

const IRExpression* IRFunctionCall::getArgumentAtN(size_t n) const noexcept {
    return arguments[n].get();
}

const std::vector<std::unique_ptr<IRTemporary>>& IRFunctionCall::getTemporaries() const noexcept {
    return temporaries;
}

void IRFunctionCall::addArgument(std::unique_ptr<IRExpression> argument, std::unique_ptr<IRTemporary> temp){
    arguments.push_back(std::move(argument));
    temporaries.push_back(std::move(temp));
}

const std::string& IRFunctionCall::getCallName() const noexcept {
    return callName;
}

void IRFunctionCall::setCallName(const std::string& _callName){
    callName = _callName;
}

size_t IRFunctionCall::getArgumentCount() const noexcept {
    return arguments.size();
}

void IRFunctionCall::print(size_t offset) const {
    std::cout << std::format("{}|-> {} | {}", std::string(offset*2, ' '), callName, toString());
    std::cout << std::format("{}|-> {}", std::string((offset+1)*2, ' '), "ARGUMENTS\n");

    for(const auto& temp : temporaries){
        if(temp != nullptr)
            temp->print(offset + 2);
    }
    for(const auto& argument : arguments){
        argument->print(offset + 2);
    }
}