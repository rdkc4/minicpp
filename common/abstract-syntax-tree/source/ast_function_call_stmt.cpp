#include "../ast_function_call_stmt.hpp"

ASTFunctionCallStmt::ASTFunctionCallStmt(const Token& token, ASTNodeType ntype) : ASTStmt(token, ntype) {}

void ASTFunctionCallStmt::initFunctionCallSt(std::unique_ptr<ASTFunctionCallExpr> funcCall) {
    functionCall = std::move(funcCall);
}

void ASTFunctionCallStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}