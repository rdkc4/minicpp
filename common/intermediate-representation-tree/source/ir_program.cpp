#include "../ir_program.hpp"

#include "../defs/ir_defs.hpp"
#include "../../preprocessing/preprocessing_libs.hpp"

ir::IRProgram::IRProgram() 
    : IRNode(ir::IRNodeType::PROGRAM) {}

const std::vector<std::unique_ptr<ir::IRFunction>>& 
ir::IRProgram::getFunctions() const noexcept {
    return functions;
}

const ir::IRFunction* 
ir::IRProgram::getFunctionAtN(size_t n) const noexcept {
    return functions[n].get();
}

void ir::IRProgram::resizeFunctions(size_t n){
    functions.resize(n);
}

void ir::IRProgram::setFunctionAtN(std::unique_ptr<IRFunction> function, size_t n){
    functions[n] = std::move(function);
}

size_t ir::IRProgram::getFunctionCount() const noexcept {
    return functions.size();
}

void ir::IRProgram::addLinkedLib(const std::string& libName) {
    linkedLibs.insert(preprocessing::generateLibObjPath(libName));
}

std::vector<std::string> ir::IRProgram::getLinkedLibs() const noexcept {
    return std::vector<std::string>(linkedLibs.begin(), linkedLibs.end());
}

void ir::IRProgram::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}