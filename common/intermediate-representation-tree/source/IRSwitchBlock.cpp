#include "../IRSwitchBlock.hpp"

#include <memory>
#include <vector>
#include <iostream>
#include <format>
#include <string>

IRSwitchBlock::IRSwitchBlock(IRNodeType ntype) : IRStatement(ntype) {}

const std::vector<std::unique_ptr<IRStatement>>& IRSwitchBlock::getStatements() const noexcept {
    return statements;
}

void IRSwitchBlock::addStatement(std::unique_ptr<IRStatement> statement){
    statements.push_back(std::move(statement));
}

void IRSwitchBlock::eliminateDead(size_t startIdx){
    if(startIdx < statements.size()){
        statements.erase(statements.begin() + startIdx, statements.end());
    }
}    

void IRSwitchBlock::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    for(const auto& statement : statements){
        statement->print(offset + 1);
    }
}