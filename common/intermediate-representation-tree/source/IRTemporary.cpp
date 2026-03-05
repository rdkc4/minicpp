#include "../IRTemporary.hpp"

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