#include "../ir_function.hpp"

#include "../defs/ir_defs.hpp"

IRFunction::IRFunction(const std::string& funcName, Type type) : IRNode(IRNodeType::FUNCTION), functionName{ funcName }, requiredMemory{ "0" }, type{ type }, predefined{ false } {}

const std::vector<std::unique_ptr<IRParameter>>& IRFunction::getParameters() const noexcept {
    return parameters;
}

void IRFunction::addParameter(std::unique_ptr<IRParameter> parameter){
    parameters.push_back(std::move(parameter));
}

const std::vector<std::unique_ptr<IRStmt>>& IRFunction::getBody() const noexcept {
    return body;
}

void IRFunction::addStatement(std::unique_ptr<IRStmt> stmt){
    body.push_back(std::move(stmt));
}

const std::string& IRFunction::getFunctionName() const noexcept {
    return functionName;
}

void IRFunction::setFunctionName(const std::string& funcName){
    functionName = funcName;
}

Type IRFunction::getType() const noexcept {
    return type;
}

void IRFunction::setType(Type returnType) noexcept {
    type = returnType;
}

const std::string& IRFunction::getRequiredMemory() const noexcept {
    return requiredMemory;
}

void IRFunction::setRequiredMemory(const std::string& size){
    requiredMemory = size;
}

void IRFunction::eliminateDeadStmts(size_t startIdx){
    if(startIdx < body.size()){
        body.erase(body.begin() + startIdx, body.end());
    }
}

void IRFunction::setPredefined(bool isPredefined) noexcept {
    predefined = isPredefined;
}

bool IRFunction::isPredefined() const noexcept {
    return predefined;
}

void IRFunction::accept(IRVisitor& visitor){
    visitor.visit(this);
}