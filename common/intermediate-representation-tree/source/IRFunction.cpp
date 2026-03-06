#include "../IRFunction.hpp"

IRFunction::IRFunction(IRNodeType ntype, const std::string& funcName, Types type) : IRNode(ntype), functionName{ funcName }, requiredMemory{ "0" }, type{ type }, predefined{ false } {}

const std::vector<std::unique_ptr<IRParameter>>& IRFunction::getParameters() const noexcept {
    return parameters;
}

void IRFunction::addParameter(std::unique_ptr<IRParameter> parameter){
    parameters.push_back(std::move(parameter));
}

const std::vector<std::unique_ptr<IRStatement>>& IRFunction::getBody() const noexcept {
    return body;
}

void IRFunction::addStatement(std::unique_ptr<IRStatement> statement){
    body.push_back(std::move(statement));
}

const std::string& IRFunction::getFunctionName() const noexcept {
    return functionName;
}

void IRFunction::setFunctionName(const std::string& funcName){
    functionName = funcName;
}

Types IRFunction::getType() const noexcept {
    return type;
}

void IRFunction::setType(Types t) noexcept {
    type = t;
}

const std::string& IRFunction::getRequiredMemory() const noexcept {
    return requiredMemory;
}

void IRFunction::setRequiredMemory(const std::string& size){
    requiredMemory = size;
}

void IRFunction::eliminateDead(size_t startIdx){
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