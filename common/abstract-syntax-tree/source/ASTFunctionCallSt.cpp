#include "../ASTFunctionCallSt.hpp"

ASTFunctionCallSt::ASTFunctionCallSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

void ASTFunctionCallSt::initFunctionCallSt(std::unique_ptr<ASTFunctionCall> funcCall) {
    functionCall = std::move(funcCall);
}