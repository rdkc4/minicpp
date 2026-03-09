#include "../ir_function_call_expr.hpp"

IRFunctionCallExpr::IRFunctionCallExpr(IRNodeType ntype, const std::string& callName, Type type) : IRExpr(ntype, type), callName{ callName } {}

const std::vector<std::unique_ptr<IRExpr>>& IRFunctionCallExpr::getArguments() const noexcept {
    return arguments;
}

const IRExpr* IRFunctionCallExpr::getArgumentAtN(size_t n) const noexcept {
    return arguments[n].get();
}

const std::vector<std::unique_ptr<IRTemporaryExpr>>& IRFunctionCallExpr::getTemporaries() const noexcept {
    return temporaries;
}

void IRFunctionCallExpr::addArgument(std::unique_ptr<IRExpr> argument, std::unique_ptr<IRTemporaryExpr> temp){
    arguments.push_back(std::move(argument));
    temporaries.push_back(std::move(temp));
}

const std::string& IRFunctionCallExpr::getCallName() const noexcept {
    return callName;
}

void IRFunctionCallExpr::setCallName(const std::string& _callName){
    callName = _callName;
}

size_t IRFunctionCallExpr::getArgumentCount() const noexcept {
    return arguments.size();
}

void IRFunctionCallExpr::accept(IRVisitor& visitor){
    visitor.visit(this);
}