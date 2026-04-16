#include "../ir_program.hpp"

#include "../defs/ir_defs.hpp"
#include "../../preprocessing/preprocessing_libs.hpp"

IR::node::IRProgram::IRProgram() 
    : IRNode(IR::defs::IRNodeType::PROGRAM) {}

const std::vector<std::unique_ptr<IR::node::IRFunction>>& 
IR::node::IRProgram::getFunctions() const noexcept {
    return functions;
}

const IR::node::IRFunction* 
IR::node::IRProgram::getFunctionAtN(size_t n) const noexcept {
    return functions[n].get();
}

void IR::node::IRProgram::resizeFunctions(size_t n){
    functions.resize(n);
}

void IR::node::IRProgram::setFunctionAtN(std::unique_ptr<IRFunction> function, size_t n){
    functions[n] = std::move(function);
}

size_t IR::node::IRProgram::getFunctionCount() const noexcept {
    return functions.size();
}

void IR::node::IRProgram::addLinkedLib(const std::string& libName) {
    linkedLibs.insert(Preprocessing::Libs::generateLibObjPath(libName));
}

std::vector<std::string> IR::node::IRProgram::getLinkedLibs() const noexcept {
    return std::vector<std::string>(linkedLibs.begin(), linkedLibs.end());
}

void IR::node::IRProgram::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}