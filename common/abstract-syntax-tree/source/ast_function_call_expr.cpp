#include "../ast_function_call_expr.hpp"

#include "../defs/ast_defs.hpp"

ASTFunctionCallExpr::ASTFunctionCallExpr(const Token& token, Type type) : ASTExpr(token, ASTNodeType::FUNCTION_CALL, type) {}

const std::vector<std::unique_ptr<ASTExpr>>& ASTFunctionCallExpr::getArguments() const noexcept {
    return arguments;
}

const ASTExpr* ASTFunctionCallExpr::getArgumentAtN(size_t n) const noexcept{
    return arguments[n].get();
}

void ASTFunctionCallExpr::addArgument(std::unique_ptr<ASTExpr> expr){
    arguments.push_back(std::move(expr));
}

size_t ASTFunctionCallExpr::getArgumentCount() const noexcept {
    return arguments.size();
}

void ASTFunctionCallExpr::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}