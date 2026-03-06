#include "../ASTFunctionCall.hpp"

ASTFunctionCall::ASTFunctionCall(const Token& token, ASTNodeType ntype, Types type) : ASTExpression(token, ntype, type) {}

const std::vector<std::unique_ptr<ASTExpression>>& ASTFunctionCall::getArguments() const noexcept {
    return arguments;
}

const ASTExpression* ASTFunctionCall::getArgumentAtN(size_t n) const noexcept{
    return arguments[n].get();
}

void ASTFunctionCall::addArgument(std::unique_ptr<ASTExpression> arg){
    arguments.push_back(std::move(arg));
}

size_t ASTFunctionCall::getArgumentCount() const noexcept {
    return arguments.size();
}