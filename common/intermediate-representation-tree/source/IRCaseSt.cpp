#include "../IRCaseSt.hpp"

#include <memory>
#include <iostream>
#include <format>
#include <string>

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

void IRCaseSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    literal->print(offset + 1);
    swBlock->print(offset + 1);
    if(breaks){
        std::cout << std::format("{}|-> {}", std::string((offset+1)*2, ' '), "BREAK\n");
    }
}