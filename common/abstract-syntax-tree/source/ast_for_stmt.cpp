#include "../ast_for_stmt.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTForStmt::ASTForStmt(const syntax::Token& token) 
    : ASTStmt(token, syntax::ast::ASTNodeType::FOR_STMT) {}

void syntax::ast::ASTForStmt::setForStmt(
    std::unique_ptr<syntax::ast::ASTAssignStmt> initStmt, 
    std::unique_ptr<syntax::ast::ASTExpr> condExpr, 
    std::unique_ptr<syntax::ast::ASTAssignStmt> incStmt, 
    std::unique_ptr<syntax::ast::ASTStmt> statement
){
    initializerStmt = std::move(initStmt);
    conditionExpr = std::move(condExpr);
    incrementerStmt = std::move(incStmt);
    stmt = std::move(statement);
}

bool syntax::ast::ASTForStmt::hasInitializerStmt() const noexcept {
    return initializerStmt != nullptr;
}

bool syntax::ast::ASTForStmt::hasConditionExpr() const noexcept {
    return conditionExpr != nullptr;
}

bool syntax::ast::ASTForStmt::hasIncrementerStmt() const noexcept {
    return incrementerStmt != nullptr;
}

void syntax::ast::ASTForStmt::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}