#include "../IRDefaultSt.hpp"

#include <memory>
#include <string>
#include <iostream>
#include <format>

IRDefaultSt::IRDefaultSt(IRNodeType ntype) : IRStatement(ntype) {}

const IRSwitchBlock* IRDefaultSt::getSwitchBlock() const noexcept {
    return swBlock.get();
}

IRSwitchBlock* IRDefaultSt::getSwitchBlockNC() noexcept {
    return swBlock.get();
}

void IRDefaultSt::setSwitchBlock(std::unique_ptr<IRSwitchBlock> block){
    swBlock = std::move(block);
}

void IRDefaultSt::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    swBlock->print(offset + 1);
}