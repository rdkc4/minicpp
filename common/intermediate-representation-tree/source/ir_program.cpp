#include "../ir_program.hpp"

#include "../defs/ir_defs.hpp"
#include "../../preprocessing/preprocessing_libraries.hpp"

IRProgram::IRProgram() : IRNode(IRNodeType::PROGRAM) {}

const std::vector<std::unique_ptr<IRFunction>>& IRProgram::getFunctions() const noexcept {
    return functions;
}

const IRFunction* IRProgram::getFunctionAtN(size_t n) const noexcept {
    return functions[n].get();
}

void IRProgram::resizeFunctions(size_t n){
    functions.resize(n);
}

void IRProgram::setFunctionAtN(std::unique_ptr<IRFunction> function, size_t n){
    functions[n] = std::move(function);
}

size_t IRProgram::getFunctionCount() const noexcept {
    return functions.size();
}

void IRProgram::addLinkedLib(const std::string& libName) {
    linkedLibs.insert(Preprocessing::Libs::generateLibObjPath(libName));
}

std::string IRProgram::getLinkedLibs() const noexcept {
    std::string libs{};
    for(const auto& lib : linkedLibs) {
        libs += " " + lib;
    }

    return libs;
}

void IRProgram::accept(IRVisitor& visitor){
    visitor.visit(this);
}