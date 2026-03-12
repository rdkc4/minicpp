#include "../ast_switch_stmt.hpp"

#include "../defs/ast_defs.hpp"

ASTSwitchStmt::ASTSwitchStmt(const Token& token) : ASTStmt(token, ASTNodeType::SWITCH_STATEMENT) {}

void ASTSwitchStmt::setVariableIdExpr(std::unique_ptr<ASTIdExpr> idExpr){
    variableIdExpr = std::move(idExpr);
}

const std::vector<std::unique_ptr<ASTCaseStmt>>& ASTSwitchStmt::getCaseStmts() const noexcept {
    return caseStmts;
}

void ASTSwitchStmt::addCaseStmt(std::unique_ptr<ASTCaseStmt> caseStmt){
    caseStmts.push_back(std::move(caseStmt));
}

void ASTSwitchStmt::setDefaultStmt(std::unique_ptr<ASTDefaultStmt> swDefaultStmt){
    defaultStmt = std::move(swDefaultStmt);
}

bool ASTSwitchStmt::hasDefaultStmt() const noexcept {
    return defaultStmt != nullptr;
}

void ASTSwitchStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}