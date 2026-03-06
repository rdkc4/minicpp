#include "../IRIfSt.hpp"

IRIfSt::IRIfSt(IRNodeType ntype) : IRStatement(ntype) {}

const std::vector<std::unique_ptr<IRExpression>>& IRIfSt::getConditions() const noexcept {
    return conditions;
}

const std::vector<std::unique_ptr<IRStatement>>& IRIfSt::getStatements() const noexcept {
    return statements;
}

const std::vector<std::unique_ptr<IRTemporary>>& IRIfSt::getTemporaries() const noexcept {
    return temporaries;
}

size_t IRIfSt::getConditionCount() const noexcept {
    return conditions.size();
}

void IRIfSt::addIf(std::unique_ptr<IRExpression> cond, std::unique_ptr<IRStatement> statement, std::unique_ptr<IRTemporary> temp){
    conditions.push_back(std::move(cond));
    statements.push_back(std::move(statement));
    temporaries.push_back(std::move(temp));
}

void IRIfSt::addElse(std::unique_ptr<IRStatement> statement){
    statements.push_back(std::move(statement));
}

bool IRIfSt::hasElse() const noexcept {
    return statements.size() > conditions.size();
}

const IRStatement* IRIfSt::getElseStatement() const noexcept {
    return hasElse() ? statements.back().get() : nullptr;
}

const std::tuple<const IRExpression*, const IRStatement*, const IRTemporary*> IRIfSt::getIfAtN(size_t n) const noexcept {
    return {conditions[n].get(), statements[n].get(), temporaries[n].get() };
}