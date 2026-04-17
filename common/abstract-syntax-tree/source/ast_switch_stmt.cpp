#include "../ast_switch_stmt.hpp"

#include "../defs/ast_defs.hpp"

syntax::ast::ASTSwitchStmt::ASTSwitchStmt(const syntax::Token& token) 
    : ASTStmt(token, syntax::ast::ASTNodeType::SWITCH_STMT) {}

void syntax::ast::ASTSwitchStmt::setVariableIdExpr(std::unique_ptr<syntax::ast::ASTIdExpr> idExpr){
    variableIdExpr = std::move(idExpr);
}

const std::vector<std::unique_ptr<syntax::ast::ASTCaseStmt>>& 
syntax::ast::ASTSwitchStmt::getCaseStmts() const noexcept {
    return caseStmts;
}

void syntax::ast::ASTSwitchStmt::addCaseStmt(std::unique_ptr<syntax::ast::ASTCaseStmt> caseStmt){
    caseStmts.push_back(std::move(caseStmt));
}

void syntax::ast::ASTSwitchStmt::setDefaultStmt(std::unique_ptr<syntax::ast::ASTDefaultStmt> swDefaultStmt){
    defaultStmt = std::move(swDefaultStmt);
}

bool syntax::ast::ASTSwitchStmt::hasDefaultStmt() const noexcept {
    return defaultStmt != nullptr;
}

void syntax::ast::ASTSwitchStmt::accept(syntax::ast::ASTVisitor& visitor) {
    visitor.visit(this);
}