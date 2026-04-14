#include "../ir_temporary_expr.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRTemporaryExpr::IRTemporaryExpr() : IRNode(IR::defs::IRNodeType::TEMPORARY) {}

const std::vector<std::unique_ptr<IR::node::IRExpr>>& IR::node::IRTemporaryExpr::getTemporaryExprs() const noexcept {
    return temporaryExprs;
}

const std::vector<std::string>& IR::node::IRTemporaryExpr::getTemporaryNames() const noexcept {
    return temporaryNames;
}

const std::string& IR::node::IRTemporaryExpr::getTemporaryNameAtN(size_t n) const noexcept {
    return temporaryNames[n];
}

const IR::node::IRExpr* IR::node::IRTemporaryExpr::getTemporaryExprAtN(size_t n) const noexcept {
    return temporaryExprs[n].get();
}

std::pair<std::string, IR::node::IRExpr*> IR::node::IRTemporaryExpr::getTemporaryDetailsAtN(size_t n) noexcept {
    return {temporaryNames[n], temporaryExprs[n].get()};
}

void IR::node::IRTemporaryExpr::addTemporaryExpr(const std::string& tempName, std::unique_ptr<IRExpr> tempVal, Type type){
    temporaryNames.push_back(tempName);
    temporaryExprs.push_back(std::move(tempVal));
    types.push_back(type);
}

void IR::node::IRTemporaryExpr::setTemporaryExprAtN(std::unique_ptr<IRExpr> tempVal, Type t, size_t n){
    temporaryExprs[n] = std::move(tempVal);
    types[n] = t;
}

const std::vector<Type>& IR::node::IRTemporaryExpr::getTypes() const noexcept {
    return types;
}

void IR::node::IRTemporaryExpr::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}