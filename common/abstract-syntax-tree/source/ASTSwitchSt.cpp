#include "../ASTSwitchSt.hpp"

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <format>

ASTSwitchSt::ASTSwitchSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

void ASTSwitchSt::setVariable(std::unique_ptr<ASTId> var){
    variable = std::move(var);
}

const std::vector<std::unique_ptr<ASTCaseSt>>& ASTSwitchSt::getCases() const noexcept {
    return cases;
}

const ASTDefaultSt* ASTSwitchSt::getDefault() const noexcept {
    return _default.get();
}

void ASTSwitchSt::addCase(std::unique_ptr<ASTCaseSt> _case){
    cases.push_back(std::move(_case));
}

void ASTSwitchSt::setDefault(std::unique_ptr<ASTDefaultSt> _swDefault){
    _default = std::move(_swDefault);
}

bool ASTSwitchSt::hasDefault() const noexcept {
    return _default != nullptr;
}

void ASTSwitchSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    variable->print(offset + 1);
    
    for(const auto& _case : cases){
        _case->print(offset + 1);
    }
    if(_default != nullptr){
        _default->print(offset + 1);
    }
}