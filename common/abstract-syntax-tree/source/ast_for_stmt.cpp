#include "../ast_for_stmt.hpp"

ASTForStmt::ASTForStmt(const Token& token, ASTNodeType ntype) : ASTStmt(token, ntype) {}

const ASTAssignStmt* ASTForStmt::getInitializer() const noexcept {
    return initializer.get();
}

const ASTAssignStmt* ASTForStmt::getIncrementer() const noexcept{
    return incrementer.get();
}

const ASTStmt* ASTForStmt::getStatement() const noexcept {
    return statement.get();
}

void ASTForStmt::setForSt(std::unique_ptr<ASTAssignStmt> init, std::unique_ptr<ASTExpr> cond, std::unique_ptr<ASTAssignStmt> inc, std::unique_ptr<ASTStmt> st){
    initializer = std::move(init);
    condition = std::move(cond);
    incrementer = std::move(inc);
    statement = std::move(st);
}

bool ASTForStmt::hasInitializer() const noexcept {
    return initializer != nullptr;
}

bool ASTForStmt::hasCondition() const noexcept {
    return condition != nullptr;
}

bool ASTForStmt::hasIncrementer() const noexcept {
    return incrementer != nullptr;
}

void ASTForStmt::accept(ASTVisitor& visitor) {
    visitor.visit(this);
}