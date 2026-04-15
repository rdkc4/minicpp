#include "../ast_if_stmt.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTIfStmt::ASTIfStmt(const Token& token) 
    : ASTStmt(token, AST::defs::ASTNodeType::IF_STMT) {}

const std::vector<std::unique_ptr<AST::node::ASTExpr>>& AST::node::ASTIfStmt::getConditionExprs() const noexcept {
    return conditionExprs;
}
const std::vector<std::unique_ptr<AST::node::ASTStmt>>& AST::node::ASTIfStmt::getStmts() const noexcept {
    return stmts;
}

void AST::node::ASTIfStmt::addIfStmt(std::unique_ptr<AST::node::ASTExpr> condition, std::unique_ptr<AST::node::ASTStmt> statement){
    conditionExprs.push_back(std::move(condition));
    stmts.push_back(std::move(statement));
}

void AST::node::ASTIfStmt::addElseStmt(std::unique_ptr<AST::node::ASTStmt> statement){
    stmts.push_back(std::move(statement));
}

bool AST::node::ASTIfStmt::hasElseStmt() const noexcept {
    return stmts.size() > conditionExprs.size();
}

void AST::node::ASTIfStmt::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}