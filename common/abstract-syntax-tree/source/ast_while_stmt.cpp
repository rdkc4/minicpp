#include "../ast_while_stmt.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTWhileStmt::ASTWhileStmt(const syntax::Token& token) 
    : ASTStmt(token, syntax::ast::ASTNodeType::WHILE_STMT) {}

void syntax::ast::ASTWhileStmt::setWhileStmt(
    std::unique_ptr<syntax::ast::ASTExpr> cond, 
    std::unique_ptr<syntax::ast::ASTStmt> statement
){
    conditionExpr = std::move(cond);
    stmt = std::move(statement);
}

void syntax::ast::ASTWhileStmt::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}