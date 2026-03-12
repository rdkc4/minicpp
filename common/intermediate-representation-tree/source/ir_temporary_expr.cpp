#include "../ir_temporary_expr.hpp"

#include "../defs/ir_defs.hpp"

IRTemporaryExpr::IRTemporaryExpr() : IRNode(IRNodeType::TEMPORARY) {}

const std::vector<std::unique_ptr<IRExpr>>& IRTemporaryExpr::getTemporaryExprs() const noexcept {
    return temporaryExprs;
}

const std::vector<std::string>& IRTemporaryExpr::getTemporaryNames() const noexcept {
    return temporaryNames;
}

const std::string& IRTemporaryExpr::getTemporaryNameAtN(size_t n) const noexcept {
    return temporaryNames[n];
}

const IRExpr* IRTemporaryExpr::getTemporaryExprAtN(size_t n) const noexcept {
    return temporaryExprs[n].get();
}

std::pair<std::string, IRExpr*> IRTemporaryExpr::getTemporaryDetailsAtN(size_t n) noexcept {
    return {temporaryNames[n], temporaryExprs[n].get()};
}

void IRTemporaryExpr::addTemporaryExpr(const std::string& tempName, std::unique_ptr<IRExpr> tempVal, Type type){
    temporaryNames.push_back(tempName);
    temporaryExprs.push_back(std::move(tempVal));
    types.push_back(type);
}

void IRTemporaryExpr::setTemporaryExprAtN(std::unique_ptr<IRExpr> tempVal, Type t, size_t n){
    temporaryExprs[n] = std::move(tempVal);
    types[n] = t;
}

const std::vector<Type>& IRTemporaryExpr::getTypes() const noexcept {
    return types;
}

void IRTemporaryExpr::accept(IRVisitor& visitor){
    visitor.visit(this);
}