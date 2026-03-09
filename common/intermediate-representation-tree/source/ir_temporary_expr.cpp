#include "../ir_temporary_expr.hpp"

IRTemporaryExpr::IRTemporaryExpr(IRNodeType ntype) : IRNode(ntype) {}

const std::vector<std::unique_ptr<IRExpr>>& IRTemporaryExpr::getTemporaries() const noexcept {
    return temporaries;
}

const std::vector<std::string>& IRTemporaryExpr::getTemporaryNames() const noexcept {
    return tempNames;
}

const std::string& IRTemporaryExpr::getTemporaryNameAtN(size_t n) const noexcept {
    return tempNames[n];
}

const IRExpr* IRTemporaryExpr::getExpressionAtN(size_t n) const noexcept {
    return temporaries[n].get();
}

std::pair<std::string, IRExpr*> IRTemporaryExpr::getTemporaryNCAtN(size_t n) noexcept {
    return {tempNames[n], temporaries[n].get()};
}

void IRTemporaryExpr::addTemporary(const std::string& tempName, std::unique_ptr<IRExpr> tempVal, Type type){
    tempNames.push_back(tempName);
    temporaries.push_back(std::move(tempVal));
    types.push_back(type);
}

void IRTemporaryExpr::assignTempAtN(std::unique_ptr<IRExpr> tempVal, Type t, size_t n){
    temporaries[n] = std::move(tempVal);
    types[n] = t;
}

const std::vector<Type>& IRTemporaryExpr::getTypes() const noexcept {
    return types;
}

void IRTemporaryExpr::accept(IRVisitor& visitor){
    visitor.visit(this);
}