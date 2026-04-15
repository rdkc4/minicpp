#include "../ast_for_stmt.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTForStmt::ASTForStmt(const Token& token) 
    : ASTStmt(token, AST::defs::ASTNodeType::FOR_STMT) {}

void AST::node::ASTForStmt::setForSt(std::unique_ptr<AST::node::ASTAssignStmt> initStmt, std::unique_ptr<AST::node::ASTExpr> condExpr, std::unique_ptr<AST::node::ASTAssignStmt> incStmt, std::unique_ptr<AST::node::ASTStmt> statement){
    initializerStmt = std::move(initStmt);
    conditionExpr = std::move(condExpr);
    incrementerStmt = std::move(incStmt);
    stmt = std::move(statement);
}

bool AST::node::ASTForStmt::hasInitializerStmt() const noexcept {
    return initializerStmt != nullptr;
}

bool AST::node::ASTForStmt::hasConditionExpr() const noexcept {
    return conditionExpr != nullptr;
}

bool AST::node::ASTForStmt::hasIncrementerStmt() const noexcept {
    return incrementerStmt != nullptr;
}

void AST::node::ASTForStmt::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}