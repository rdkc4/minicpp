#include "../ir_temporary_expr.hpp"

#include "../defs/ir_defs.hpp"

ir::IRTemporaryExpr::IRTemporaryExpr() 
    : IRNode(ir::IRNodeType::TEMPORARY) {}

const std::vector<std::unique_ptr<ir::IRExpr>>& 
ir::IRTemporaryExpr::getTemporaryExprs() const noexcept {
    return temporaryExprs;
}

const std::vector<std::string>& 
ir::IRTemporaryExpr::getTemporaryNames() const noexcept {
    return temporaryNames;
}

const std::string& 
ir::IRTemporaryExpr::getTemporaryNameAtN(size_t n) const noexcept {
    return temporaryNames[n];
}

const ir::IRExpr* 
ir::IRTemporaryExpr::getTemporaryExprAtN(size_t n) const noexcept {
    return temporaryExprs[n].get();
}

std::pair<std::string, ir::IRExpr*> 
ir::IRTemporaryExpr::getTemporaryDetailsAtN(size_t n) noexcept {
    return {temporaryNames[n], temporaryExprs[n].get()};
}

void ir::IRTemporaryExpr::addTemporaryExpr(
    const std::string& tempName, 
    std::unique_ptr<IRExpr> tempVal, 
    types::Type type
){
    temporaryNames.push_back(tempName);
    temporaryExprs.push_back(std::move(tempVal));
    types.push_back(type);
}

void ir::IRTemporaryExpr::setTemporaryExprAtN(
    std::unique_ptr<IRExpr> tempVal, 
    types::Type type, 
    size_t n
){
    temporaryExprs[n] = std::move(tempVal);
    types[n] = type;
}

const std::vector<types::Type>& ir::IRTemporaryExpr::getTypes() const noexcept {
    return types;
}

void ir::IRTemporaryExpr::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}