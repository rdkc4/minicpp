#include "../ast_function_call_stmt.hpp"

#include "../defs/ast_defs.hpp"

ASTFunctionCallStmt::ASTFunctionCallStmt(const Token& token) 
    : ASTStmt(token, ASTNodeType::FUNCTION_CALL_STMT) {}

void ASTFunctionCallStmt::setFunctionCallStmt(std::unique_ptr<ASTFunctionCallExpr> callExpr) {
    functionCallExpr = std::move(callExpr);
}

void ASTFunctionCallStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}