#include "../IRDoWhileSt.hpp"

IRDoWhileSt::IRDoWhileSt(IRNodeType ntype) : IRStatement(ntype) {}

const IRExpression* IRDoWhileSt::getCondition() const noexcept {
    return condition.get();
}

void IRDoWhileSt::setDoWhileSt(std::unique_ptr<IRExpression> cond, std::unique_ptr<IRStatement> st, std::unique_ptr<IRTemporary> temp){
    condition = std::move(cond);
    statement = std::move(st);
    temporaries = std::move(temp);
}

const IRTemporary* IRDoWhileSt::getTemporaries() const noexcept {
    return temporaries.get();
}

bool IRDoWhileSt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}