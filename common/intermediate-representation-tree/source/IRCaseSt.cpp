#include "../IRCaseSt.hpp"

IRCaseSt::IRCaseSt(IRNodeType ntype) : IRStatement(ntype) {}

const IRLiteral* IRCaseSt::getLiteral() const noexcept {
    return literal.get();
}

void IRCaseSt::setCase(std::unique_ptr<IRLiteral> lit, std::unique_ptr<IRSwitchBlock> block, bool hasBr){
    literal = std::move(lit);
    swBlock = std::move(block);
    breaks = hasBr;
}

bool IRCaseSt::hasBreak() const noexcept {
    return breaks;
}