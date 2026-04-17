#include "../ast_program.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTProgram::ASTProgram(const syntax::Token& token) 
    : ASTNode(token, syntax::ast::ASTNodeType::PROGRAM) {}

const std::vector<std::unique_ptr<syntax::ast::ASTFunction>>& 
syntax::ast::ASTProgram::getFunctions() const noexcept {
    return functions;
}

size_t syntax::ast::ASTProgram::getFunctionCount() const noexcept {
    return functions.size();
}

const syntax::ast::ASTFunction* syntax::ast::ASTProgram::getFunctionAtN(size_t n) const noexcept {
    return functions[n].get();
}

const std::vector<std::unique_ptr<syntax::ast::ASTDir>>& 
syntax::ast::ASTProgram::getDirs() const noexcept {
    return dirs;
}

const syntax::ast::ASTDir* syntax::ast::ASTProgram::getDirAtN(size_t n) const noexcept {
    return dirs[n].get();
}

void syntax::ast::ASTProgram::addFunction(std::unique_ptr<syntax::ast::ASTFunction> function){
    functions.push_back(std::move(function));
}

void syntax::ast::ASTProgram::addDir(std::unique_ptr<syntax::ast::ASTDir> directive) {
    dirs.push_back(std::move(directive));
}

void syntax::ast::ASTProgram::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}