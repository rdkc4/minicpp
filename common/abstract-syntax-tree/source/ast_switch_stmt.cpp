#include "../ast_switch_stmt.hpp"

#include "../defs/ast_defs.hpp"

AST::node::ASTSwitchStmt::ASTSwitchStmt(const Token& token) 
    : ASTStmt(token, AST::defs::ASTNodeType::SWITCH_STMT) {}

void AST::node::ASTSwitchStmt::setVariableIdExpr(std::unique_ptr<AST::node::ASTIdExpr> idExpr){
    variableIdExpr = std::move(idExpr);
}

const std::vector<std::unique_ptr<AST::node::ASTCaseStmt>>& 
AST::node::ASTSwitchStmt::getCaseStmts() const noexcept {
    return caseStmts;
}

void AST::node::ASTSwitchStmt::addCaseStmt(std::unique_ptr<AST::node::ASTCaseStmt> caseStmt){
    caseStmts.push_back(std::move(caseStmt));
}

void AST::node::ASTSwitchStmt::setDefaultStmt(std::unique_ptr<AST::node::ASTDefaultStmt> swDefaultStmt){
    defaultStmt = std::move(swDefaultStmt);
}

bool AST::node::ASTSwitchStmt::hasDefaultStmt() const noexcept {
    return defaultStmt != nullptr;
}

void AST::node::ASTSwitchStmt::accept(AST::visitor::ASTVisitor& visitor) {
    visitor.visit(this);
}