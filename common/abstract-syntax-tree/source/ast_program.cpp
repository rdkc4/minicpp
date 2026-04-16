#include "../ast_program.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTProgram::ASTProgram(const Token& token) 
    : ASTNode(token, AST::defs::ASTNodeType::PROGRAM) {}

const std::vector<std::unique_ptr<AST::node::ASTFunction>>& 
AST::node::ASTProgram::getFunctions() const noexcept {
    return functions;
}

size_t AST::node::ASTProgram::getFunctionCount() const noexcept {
    return functions.size();
}

const AST::node::ASTFunction* AST::node::ASTProgram::getFunctionAtN(size_t n) const noexcept {
    return functions[n].get();
}

const std::vector<std::unique_ptr<AST::node::ASTDir>>& 
AST::node::ASTProgram::getDirs() const noexcept {
    return dirs;
}

const AST::node::ASTDir* AST::node::ASTProgram::getDirAtN(size_t n) const noexcept {
    return dirs[n].get();
}

void AST::node::ASTProgram::addFunction(std::unique_ptr<AST::node::ASTFunction> function){
    functions.push_back(std::move(function));
}

void AST::node::ASTProgram::addDir(std::unique_ptr<AST::node::ASTDir> directive) {
    dirs.push_back(std::move(directive));
}

void AST::node::ASTProgram::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}