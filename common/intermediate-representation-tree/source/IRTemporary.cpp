#include "../IRTemporary.hpp"

#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <format>
#include <utility>

IRTemporary::IRTemporary(IRNodeType ntype) : IRNode(ntype) {}

const std::vector<std::unique_ptr<IRExpression>>& IRTemporary::getTemporaries() const noexcept {
    return temporaries;
}

const std::vector<std::string>& IRTemporary::getTemporaryNames() const noexcept {
    return tempNames;
}

const std::string& IRTemporary::getTemporaryNameAtN(size_t n) const noexcept {
    return tempNames[n];
}

const IRExpression* IRTemporary::getExpressionAtN(size_t n) const noexcept {
    return temporaries[n].get();
}

std::pair<std::string, IRExpression*> IRTemporary::getTemporaryNCAtN(size_t n) noexcept {
    return {tempNames[n], temporaries[n].get()};
}

void IRTemporary::addTemporary(const std::string& tempName, std::unique_ptr<IRExpression> tempVal, Types type){
    tempNames.push_back(tempName);
    temporaries.push_back(std::move(tempVal));
    types.push_back(type);
}

void IRTemporary::assignTempAtN(std::unique_ptr<IRExpression> tempVal, Types t, size_t n){
    temporaries[n] = std::move(tempVal);
    types[n] = t;
}

const std::vector<Types>& IRTemporary::getTypes() const noexcept {
    return types;
}

void IRTemporary::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    const size_t tempCount = temporaries.size();
    for(size_t i = 0; i < tempCount; ++i){
        std::cout << std::format("{}|-> {}\n", std::string((offset+1) * 2, ' '), tempNames[i]);
        if(temporaries[i] != nullptr)
            temporaries[i]->print(offset + 2);
    }
}