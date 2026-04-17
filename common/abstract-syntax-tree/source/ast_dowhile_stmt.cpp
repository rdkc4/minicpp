#include "../ast_dowhile_stmt.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTDoWhileStmt::ASTDoWhileStmt(const syntax::Token& token) 
    : ASTStmt(token, syntax::ast::ASTNodeType::DOWHILE_STMT) {}

void syntax::ast::ASTDoWhileStmt::setDoWhile(
    std::unique_ptr<syntax::ast::ASTExpr> condExpr, 
    std::unique_ptr<syntax::ast::ASTStmt> statement
){
    conditionExpr = std::move(condExpr);
    stmt = std::move(statement);
}

void syntax::ast::ASTDoWhileStmt::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}