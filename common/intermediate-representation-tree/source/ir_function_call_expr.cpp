#include "../ir_function_call_expr.hpp"

#include "../defs/ir_defs.hpp"

ir::IRFunctionCallExpr::IRFunctionCallExpr(const std::string& callName, types::Type type) 
    : IRExpr(ir::IRNodeType::CALL, type), functionCallName{ callName } {}

const std::vector<std::unique_ptr<ir::IRExpr>>& 
ir::IRFunctionCallExpr::getArguments() const noexcept {
    return arguments;
}

const ir::IRExpr* 
ir::IRFunctionCallExpr::getArgumentAtN(size_t n) const noexcept {
    return arguments[n].get();
}

const std::vector<std::unique_ptr<ir::IRTemporaryExpr>>& 
ir::IRFunctionCallExpr::getTemporaryExprs() const noexcept {
    return temporaryExprs;
}

void ir::IRFunctionCallExpr::addArgument(
    std::unique_ptr<IRExpr> argument, 
    std::unique_ptr<IRTemporaryExpr> tempExpr
){
    arguments.push_back(std::move(argument));
    temporaryExprs.push_back(std::move(tempExpr));
}

const std::string& ir::IRFunctionCallExpr::getCallName() const noexcept {
    return functionCallName;
}

void ir::IRFunctionCallExpr::setCallName(const std::string& callName){
    functionCallName = callName;
}

size_t ir::IRFunctionCallExpr::getArgumentCount() const noexcept {
    return arguments.size();
}

void ir::IRFunctionCallExpr::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}