#include "../ast_function_call_expr.hpp"

ASTFunctionCallExpr::ASTFunctionCallExpr(const Token& token, ASTNodeType ntype, Type type) : ASTExpr(token, ntype, type) {}

const std::vector<std::unique_ptr<ASTExpr>>& ASTFunctionCallExpr::getArguments() const noexcept {
    return arguments;
}

const ASTExpr* ASTFunctionCallExpr::getArgumentAtN(size_t n) const noexcept{
    return arguments[n].get();
}

void ASTFunctionCallExpr::addArgument(std::unique_ptr<ASTExpr> arg){
    arguments.push_back(std::move(arg));
}

size_t ASTFunctionCallExpr::getArgumentCount() const noexcept {
    return arguments.size();
}

void ASTFunctionCallExpr::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}