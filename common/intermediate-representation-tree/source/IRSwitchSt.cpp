#include "../IRSwitchSt.hpp"

IRSwitchSt::IRSwitchSt(IRNodeType ntype) : IRStatement(ntype) {}

const IRId* IRSwitchSt::getVariable() const noexcept {
    return variable.get();
}

void IRSwitchSt::setVariable(std::unique_ptr<IRId> var){
    variable = std::move(var);
}

const std::vector<std::unique_ptr<IRCaseSt>>& IRSwitchSt::getCases() const noexcept {
    return cases;
}

const IRCaseSt* IRSwitchSt::getCaseAtN(size_t n) const noexcept {
    return cases[n].get();
}

void IRSwitchSt::addCase(std::unique_ptr<IRCaseSt> _case){
    cases.push_back(std::move(_case));
}

void IRSwitchSt::setDefault(std::unique_ptr<IRDefaultSt> _swDefault) {
    _default = std::move(_swDefault);
}

bool IRSwitchSt::hasDefault() const noexcept {
    return _default != nullptr;
}

size_t IRSwitchSt::getCaseCount() const noexcept {
    return cases.size();
}