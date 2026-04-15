#include "../ast_function_call_stmt.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTFunctionCallStmt::ASTFunctionCallStmt(const Token& token) 
    : ASTStmt(token, AST::defs::ASTNodeType::FUNCTION_CALL_STMT) {}

void AST::node::ASTFunctionCallStmt::setFunctionCallStmt(std::unique_ptr<AST::node::ASTFunctionCallExpr> callExpr) {
    functionCallExpr = std::move(callExpr);
}

void AST::node::ASTFunctionCallStmt::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}