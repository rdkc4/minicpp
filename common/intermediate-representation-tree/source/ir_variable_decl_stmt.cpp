#include "../ir_variable_decl_stmt.hpp"

#include "../defs/ir_defs.hpp"

IRVariableDeclStmt::IRVariableDeclStmt(std::string_view varName, Type type) 
    : IRStmt(IRNodeType::VARIABLE), varName{ varName }, value{ "0" }, type{ type } {}

void IRVariableDeclStmt::setAssignExpr(std::unique_ptr<IRExpr> expr, std::unique_ptr<IRTemporaryExpr> tempExpr){
    assignExpr = std::move(expr);
    temporaryExpr = std::move(tempExpr);
}

const std::string& IRVariableDeclStmt::getVarName() const noexcept {
    return varName;
}

void IRVariableDeclStmt::setVarName(const std::string& var){
    varName = var;
}

const std::string& IRVariableDeclStmt::getValue() const noexcept {
    return value;
}

void IRVariableDeclStmt::setValue(const std::string& val){
    value = val;
}

Type IRVariableDeclStmt::getType() const noexcept {
    return type;
}

void IRVariableDeclStmt::setType(Type t) noexcept {
    type = t;
}

bool IRVariableDeclStmt::hasAssignExpr() const noexcept {
    return assignExpr != nullptr;
}

bool IRVariableDeclStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void IRVariableDeclStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}