#include "../ir_function.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRFunction::IRFunction(const std::string& funcName, Type type) 
    : IRNode(IR::defs::IRNodeType::FUNCTION), 
      functionName{ funcName }, 
      requiredMemory{ "0" }, 
      type{ type }, 
      predefined{ false } {}

const std::vector<std::unique_ptr<IR::node::IRParameter>>& 
IR::node::IRFunction::getParameters() const noexcept {
    return parameters;
}

void IR::node::IRFunction::addParameter(std::unique_ptr<IRParameter> parameter){
    parameters.push_back(std::move(parameter));
}

const std::vector<std::unique_ptr<IR::node::IRStmt>>& 
IR::node::IRFunction::getBody() const noexcept {
    return body;
}

void IR::node::IRFunction::addStatement(std::unique_ptr<IRStmt> stmt){
    body.push_back(std::move(stmt));
}

const std::string& IR::node::IRFunction::getFunctionName() const noexcept {
    return functionName;
}

void IR::node::IRFunction::setFunctionName(const std::string& funcName){
    functionName = funcName;
}

Type IR::node::IRFunction::getType() const noexcept {
    return type;
}

void IR::node::IRFunction::setType(Type returnType) noexcept {
    type = returnType;
}

const std::string& IR::node::IRFunction::getRequiredMemory() const noexcept {
    return requiredMemory;
}

void IR::node::IRFunction::setRequiredMemory(const std::string& size){
    requiredMemory = size;
}

void IR::node::IRFunction::eliminateDeadStmts(size_t startIdx){
    if(startIdx < body.size()){
        body.erase(body.begin() + startIdx, body.end());
    }
}

void IR::node::IRFunction::setPredefined(bool isPredefined) noexcept {
    predefined = isPredefined;
}

bool IR::node::IRFunction::isPredefined() const noexcept {
    return predefined;
}

void IR::node::IRFunction::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}