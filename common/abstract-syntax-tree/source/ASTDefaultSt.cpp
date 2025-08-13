#include "../ASTDefaultSt.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <format>

ASTDefaultSt::ASTDefaultSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

const ASTSwitchBlock* ASTDefaultSt::getSwitchBlock() const noexcept {
    return swBlock.get();
}

void ASTDefaultSt::setDefault(std::unique_ptr<ASTSwitchBlock> _swBlock){
    swBlock = std::move(_swBlock);
}

void ASTDefaultSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
    swBlock->print(offset + 1);
}