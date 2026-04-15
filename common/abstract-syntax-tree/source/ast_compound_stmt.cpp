#include "../ast_compound_stmt.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTCompoundStmt::ASTCompoundStmt(const Token& token) 
    : ASTStmt(token, AST::defs::ASTNodeType::COMPOUND_STMT) {}

const std::vector<std::unique_ptr<AST::node::ASTStmt>>& AST::node::ASTCompoundStmt::getStmts() const noexcept {
    return stmts;
}

void AST::node::ASTCompoundStmt::addStmt(std::unique_ptr<AST::node::ASTStmt> stmt){
    stmts.push_back(std::move(stmt));
}

void AST::node::ASTCompoundStmt::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}