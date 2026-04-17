#include "../ast_if_stmt.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTIfStmt::ASTIfStmt(const syntax::Token& token) 
    : ASTStmt(token, syntax::ast::ASTNodeType::IF_STMT) {}

const std::vector<std::unique_ptr<syntax::ast::ASTExpr>>& 
syntax::ast::ASTIfStmt::getConditionExprs() const noexcept {
    return conditionExprs;
}
const std::vector<std::unique_ptr<syntax::ast::ASTStmt>>& 
syntax::ast::ASTIfStmt::getStmts() const noexcept {
    return stmts;
}

void syntax::ast::ASTIfStmt::addIfStmt(
    std::unique_ptr<syntax::ast::ASTExpr> condition, 
    std::unique_ptr<syntax::ast::ASTStmt> statement
){
    conditionExprs.push_back(std::move(condition));
    stmts.push_back(std::move(statement));
}

void syntax::ast::ASTIfStmt::addElseStmt(std::unique_ptr<syntax::ast::ASTStmt> statement){
    stmts.push_back(std::move(statement));
}

bool syntax::ast::ASTIfStmt::hasElseStmt() const noexcept {
    return stmts.size() > conditionExprs.size();
}

void syntax::ast::ASTIfStmt::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}