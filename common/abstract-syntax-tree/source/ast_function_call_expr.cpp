#include "../ast_function_call_expr.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTFunctionCallExpr::ASTFunctionCallExpr(const Token& token, Type type) 
    : ASTExpr(token, AST::defs::ASTNodeType::FUNCTION_CALL_EXPR, type) {}

const std::vector<std::unique_ptr<AST::node::ASTExpr>>& 
AST::node::ASTFunctionCallExpr::getArguments() const noexcept {
    return arguments;
}

const AST::node::ASTExpr* AST::node::ASTFunctionCallExpr::getArgumentAtN(size_t n) const noexcept{
    return arguments[n].get();
}

void AST::node::ASTFunctionCallExpr::addArgument(std::unique_ptr<AST::node::ASTExpr> expr){
    arguments.push_back(std::move(expr));
}

size_t AST::node::ASTFunctionCallExpr::getArgumentCount() const noexcept {
    return arguments.size();
}

void AST::node::ASTFunctionCallExpr::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}