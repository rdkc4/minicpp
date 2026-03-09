#include "../ast_switch_stmt.hpp"

ASTSwitchStmt::ASTSwitchStmt(const Token& token, ASTNodeType ntype) : ASTStmt(token, ntype) {}

void ASTSwitchStmt::setVariable(std::unique_ptr<ASTIdExpr> var){
    variable = std::move(var);
}

const std::vector<std::unique_ptr<ASTCaseStmt>>& ASTSwitchStmt::getCases() const noexcept {
    return cases;
}

void ASTSwitchStmt::addCase(std::unique_ptr<ASTCaseStmt> _case){
    cases.push_back(std::move(_case));
}

void ASTSwitchStmt::setDefault(std::unique_ptr<ASTDefaultStmt> _swDefault){
    _default = std::move(_swDefault);
}

bool ASTSwitchStmt::hasDefault() const noexcept {
    return _default != nullptr;
}

void ASTSwitchStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}