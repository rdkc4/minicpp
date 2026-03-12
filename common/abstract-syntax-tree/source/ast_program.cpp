#include "../ast_program.hpp"

#include "../defs/ast_defs.hpp"

ASTProgram::ASTProgram(const Token& token) : ASTNode(token, ASTNodeType::PROGRAM) {}

const std::vector<std::unique_ptr<ASTFunction>>& ASTProgram::getFunctions() const noexcept {
    return functions;
}

size_t ASTProgram::getFunctionCount() const noexcept {
    return functions.size();
}

const ASTFunction* ASTProgram::getFunctionAtN(size_t n) const noexcept {
    return functions[n].get();
}

const std::vector<std::unique_ptr<ASTDir>>& ASTProgram::getDirs() const noexcept {
    return dirs;
}

const ASTDir* ASTProgram::getDirAtN(size_t n) const noexcept {
    return dirs[n].get();
}

void ASTProgram::addFunction(std::unique_ptr<ASTFunction> function){
    functions.push_back(std::move(function));
}

void ASTProgram::addDir(std::unique_ptr<ASTDir> directive) {
    dirs.push_back(std::move(directive));
}

void ASTProgram::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}