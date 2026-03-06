#include "../ASTCaseSt.hpp"

ASTCaseSt::ASTCaseSt(const Token& token, ASTNodeType ntype) : ASTStatement(token, ntype) {}

const ASTLiteral* ASTCaseSt::getLiteral() const noexcept {
    return literal.get();
}

const ASTSwitchBlock* ASTCaseSt::getSwitchBlock() const noexcept {
    return swBlock.get();
}

void ASTCaseSt::setCase(std::unique_ptr<ASTLiteral> lit, std::unique_ptr<ASTSwitchBlock> _swBlock, bool hasBr){
    literal = std::move(lit);
    swBlock = std::move(_swBlock);
    _break = hasBr;
}

bool ASTCaseSt::hasBreak() const noexcept {
    return _break;
}