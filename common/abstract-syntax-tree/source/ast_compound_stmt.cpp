#include "../ast_compound_stmt.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTCompoundStmt::ASTCompoundStmt(const syntax::Token& token) 
    : ASTStmt(token, syntax::ast::ASTNodeType::COMPOUND_STMT) {}

const std::vector<std::unique_ptr<syntax::ast::ASTStmt>>& syntax::ast::ASTCompoundStmt::getStmts() const noexcept {
    return stmts;
}

void syntax::ast::ASTCompoundStmt::addStmt(std::unique_ptr<syntax::ast::ASTStmt> stmt){
    stmts.push_back(std::move(stmt));
}

void syntax::ast::ASTCompoundStmt::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}