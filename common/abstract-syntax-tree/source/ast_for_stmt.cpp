#include "../ast_for_stmt.hpp"

#include "../defs/ast_defs.hpp"

ASTForStmt::ASTForStmt(const Token& token) : ASTStmt(token, ASTNodeType::FOR_STATEMENT) {}

void ASTForStmt::setForSt(std::unique_ptr<ASTAssignStmt> initStmt, std::unique_ptr<ASTExpr> condExpr, std::unique_ptr<ASTAssignStmt> incStmt, std::unique_ptr<ASTStmt> statement){
    initializerStmt = std::move(initStmt);
    conditionExpr = std::move(condExpr);
    incrementerStmt = std::move(incStmt);
    stmt = std::move(statement);
}

bool ASTForStmt::hasInitializerStmt() const noexcept {
    return initializerStmt != nullptr;
}

bool ASTForStmt::hasConditionExpr() const noexcept {
    return conditionExpr != nullptr;
}

bool ASTForStmt::hasIncrementerStmt() const noexcept {
    return incrementerStmt != nullptr;
}

void ASTForStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}