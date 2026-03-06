#include "../IRFunctionCall.hpp"

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