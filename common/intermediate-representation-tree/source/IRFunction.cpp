#include "../IRFunction.hpp"

#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <format>

IRFunction::IRFunction(IRNodeType ntype, const std::string& funcName, Types type) : IRNode(ntype), functionName{ funcName }, requiredMemory{ "0" }, type{ type } {}

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

void IRFunction::print(size_t offset) const {
    std::cout << std::format("{}|-> {} | {}", std::string(offset*2, ' '), functionName, toString());
    
    std::cout << std::format("{}|-> {}", std::string((offset+1)*2, ' '), "PARAMETERS\n");
    for(const auto& parameter : parameters){
        parameter->print(offset + 2);
    }
    
    std::cout << std::format("{}|-> {}", std::string((offset+1)*2, ' '), "BODY\n");
    for(const auto& statement : body){
        statement->print(offset + 2);
    }   
}