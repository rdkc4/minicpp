#include "../ast_function_call_expr.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTFunctionCallExpr::ASTFunctionCallExpr(const syntax::Token& token, types::Type type) 
    : ASTExpr(token, syntax::ast::ASTNodeType::FUNCTION_CALL_EXPR, type) {}

const std::vector<std::unique_ptr<syntax::ast::ASTExpr>>& 
syntax::ast::ASTFunctionCallExpr::getArguments() const noexcept {
    return arguments;
}

const syntax::ast::ASTExpr* syntax::ast::ASTFunctionCallExpr::getArgumentAtN(size_t n) const noexcept{
    return arguments[n].get();
}

void syntax::ast::ASTFunctionCallExpr::addArgument(std::unique_ptr<syntax::ast::ASTExpr> expr){
    arguments.push_back(std::move(expr));
}

size_t syntax::ast::ASTFunctionCallExpr::getArgumentCount() const noexcept {
    return arguments.size();
}

void syntax::ast::ASTFunctionCallExpr::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}