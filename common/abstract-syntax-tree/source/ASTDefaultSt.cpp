#include "../ASTDefaultSt.hpp"

ASTDefaultSt::ASTDefaultSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

const ASTSwitchBlock* ASTDefaultSt::getSwitchBlock() const noexcept {
    return swBlock.get();
}

void ASTDefaultSt::setDefault(std::unique_ptr<ASTSwitchBlock> _swBlock){
    swBlock = std::move(_swBlock);
}