#include "../IRProgram.hpp"

#include "../../preprocessing/preprocessing_libraries.hpp"

IRProgram::IRProgram(IRNodeType ntype) : IRNode(ntype) {}

const std::vector<std::unique_ptr<IRFunction>>& IRProgram::getFunctions() const noexcept {
    return functions;
}

const IRFunction* IRProgram::getFunctionAtN(size_t n) const noexcept {
    return functions[n].get();
}

void IRProgram::resizeFunctions(size_t n){
    functions.resize(n);
}

void IRProgram::setFunctionAtN(std::unique_ptr<IRFunction> _function, size_t n){
    functions[n] = std::move(_function);
}

size_t IRProgram::getFunctionCount() const noexcept {
    return functions.size();
}

void IRProgram::addLinkedLibrary(const std::string& libName) {
    linkedLibs.insert(Preprocessing::Libs::generateLibObjPath(libName));
}

const std::string IRProgram::getLinkedLibs() const noexcept {
    std::string libs{};
    for(const auto& lib : linkedLibs) {
        libs += " " + lib;
    }

    return libs;
}