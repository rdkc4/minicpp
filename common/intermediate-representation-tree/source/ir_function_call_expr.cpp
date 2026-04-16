#include "../ir_function_call_expr.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRFunctionCallExpr::IRFunctionCallExpr(const std::string& callName, Type type) 
    : IRExpr(IR::defs::IRNodeType::CALL, type), functionCallName{ callName } {}

const std::vector<std::unique_ptr<IR::node::IRExpr>>& 
IR::node::IRFunctionCallExpr::getArguments() const noexcept {
    return arguments;
}

const IR::node::IRExpr* 
IR::node::IRFunctionCallExpr::getArgumentAtN(size_t n) const noexcept {
    return arguments[n].get();
}

const std::vector<std::unique_ptr<IR::node::IRTemporaryExpr>>& 
IR::node::IRFunctionCallExpr::getTemporaryExprs() const noexcept {
    return temporaryExprs;
}

void IR::node::IRFunctionCallExpr::addArgument(
    std::unique_ptr<IRExpr> argument, 
    std::unique_ptr<IRTemporaryExpr> tempExpr
){
    arguments.push_back(std::move(argument));
    temporaryExprs.push_back(std::move(tempExpr));
}

const std::string& IR::node::IRFunctionCallExpr::getCallName() const noexcept {
    return functionCallName;
}

void IR::node::IRFunctionCallExpr::setCallName(const std::string& callName){
    functionCallName = callName;
}

size_t IR::node::IRFunctionCallExpr::getArgumentCount() const noexcept {
    return arguments.size();
}

void IR::node::IRFunctionCallExpr::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}