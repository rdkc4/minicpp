#include "../ir_if_stmt.hpp"

IRIfStmt::IRIfStmt(IRNodeType ntype) : IRStmt(ntype) {}

const std::vector<std::unique_ptr<IRExpr>>& IRIfStmt::getConditions() const noexcept {
    return conditions;
}

const std::vector<std::unique_ptr<IRStmt>>& IRIfStmt::getStatements() const noexcept {
    return statements;
}

const std::vector<std::unique_ptr<IRTemporaryExpr>>& IRIfStmt::getTemporaries() const noexcept {
    return temporaries;
}

size_t IRIfStmt::getConditionCount() const noexcept {
    return conditions.size();
}

void IRIfStmt::addIf(std::unique_ptr<IRExpr> cond, std::unique_ptr<IRStmt> statement, std::unique_ptr<IRTemporaryExpr> temp){
    conditions.push_back(std::move(cond));
    statements.push_back(std::move(statement));
    temporaries.push_back(std::move(temp));
}

void IRIfStmt::addElse(std::unique_ptr<IRStmt> statement){
    statements.push_back(std::move(statement));
}

bool IRIfStmt::hasElse() const noexcept {
    return statements.size() > conditions.size();
}

const IRStmt* IRIfStmt::getElseStatement() const noexcept {
    return hasElse() ? statements.back().get() : nullptr;
}

const std::tuple<const IRExpr*, const IRStmt*, const IRTemporaryExpr*> IRIfStmt::getIfAtN(size_t n) const noexcept {
    return {conditions[n].get(), statements[n].get(), temporaries[n].get() };
}

void IRIfStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}