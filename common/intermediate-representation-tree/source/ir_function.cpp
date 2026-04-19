#include "../ir_function.hpp"

#include "../defs/ir_defs.hpp"

ir::IRFunction::IRFunction(const std::string& funcName, types::Type type) 
    : IRNode(ir::IRNodeType::FUNCTION), 
      functionName{ funcName }, 
      requiredMemory{ "0" }, 
      type{ type }, 
      predefined{ false } {}

const std::vector<std::unique_ptr<ir::IRParameter>>& 
ir::IRFunction::getParameters() const noexcept {
    return parameters;
}

void ir::IRFunction::addParameter(std::unique_ptr<IRParameter> parameter){
    parameters.push_back(std::move(parameter));
}

const std::vector<std::unique_ptr<ir::IRStmt>>& 
ir::IRFunction::getBody() const noexcept {
    return body;
}

void ir::IRFunction::addStatement(std::unique_ptr<IRStmt> stmt){
    body.push_back(std::move(stmt));
}

const std::string& ir::IRFunction::getFunctionName() const noexcept {
    return functionName;
}

void ir::IRFunction::setFunctionName(const std::string& funcName){
    functionName = funcName;
}

types::Type ir::IRFunction::getType() const noexcept {
    return type;
}

void ir::IRFunction::setType(types::Type returnType) noexcept {
    type = returnType;
}

const std::string& ir::IRFunction::getRequiredMemory() const noexcept {
    return requiredMemory;
}

void ir::IRFunction::setRequiredMemory(const std::string& size){
    requiredMemory = size;
}

void ir::IRFunction::eliminateDeadStmts(size_t startIdx){
    if(startIdx < body.size()){
        body.erase(body.begin() + startIdx, body.end());
    }
}

void ir::IRFunction::setPredefined(bool isPredefined) noexcept {
    predefined = isPredefined;
}

bool ir::IRFunction::isPredefined() const noexcept {
    return predefined;
}

void ir::IRFunction::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}