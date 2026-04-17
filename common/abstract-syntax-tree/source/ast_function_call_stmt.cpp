#include "../ast_function_call_stmt.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTFunctionCallStmt::ASTFunctionCallStmt(const syntax::Token& token) 
    : ASTStmt(token, syntax::ast::ASTNodeType::FUNCTION_CALL_STMT) {}

void syntax::ast::ASTFunctionCallStmt::setFunctionCallStmt(
    std::unique_ptr<syntax::ast::ASTFunctionCallExpr> callExpr
){
    functionCallExpr = std::move(callExpr);
}

void syntax::ast::ASTFunctionCallStmt::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}