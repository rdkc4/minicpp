#include "../IRDefaultSt.hpp"

IRDefaultSt::IRDefaultSt(IRNodeType ntype) : IRStatement(ntype) {}

void IRDefaultSt::setSwitchBlock(std::unique_ptr<IRSwitchBlock> block){
    swBlock = std::move(block);
}