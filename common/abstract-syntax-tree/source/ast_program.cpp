#include "../ast_program.hpp"

ASTProgram::ASTProgram(const Token& token, ASTNodeType ntype) : ASTNode(token, ntype) {}

const std::vector<std::unique_ptr<ASTFunction>>& ASTProgram::getFunctions() const noexcept {
    return functions;
}

size_t ASTProgram::getFunctionCount() const noexcept {
    return functions.size();
}

const ASTFunction* ASTProgram::getFunctionAtN(size_t n) const noexcept {
    return functions[n].get();
}

const std::vector<std::unique_ptr<ASTDir>>& ASTProgram::getDirectives() const noexcept {
    return directives;
}

const ASTDir* ASTProgram::getDirectiveAtN(size_t n) const noexcept {
    return directives[n].get();
}

void ASTProgram::addFunction(std::unique_ptr<ASTFunction> function){
    functions.push_back(std::move(function));
}

void ASTProgram::addDirective(std::unique_ptr<ASTDir> directive) {
    directives.push_back(std::move(directive));
}

void ASTProgram::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}