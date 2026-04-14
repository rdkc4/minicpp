#include "../ir_variable_decl_stmt.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRVariableDeclStmt::IRVariableDeclStmt(std::string_view varName, Type type) 
    : IRStmt(IR::defs::IRNodeType::VARIABLE), varName{ varName }, value{ "0" }, type{ type } {}

void IR::node::IRVariableDeclStmt::setAssignExpr(std::unique_ptr<IRExpr> expr, std::unique_ptr<IRTemporaryExpr> tempExpr){
    assignExpr = std::move(expr);
    temporaryExpr = std::move(tempExpr);
}

const std::string& IR::node::IRVariableDeclStmt::getVarName() const noexcept {
    return varName;
}

void IR::node::IRVariableDeclStmt::setVarName(const std::string& var){
    varName = var;
}

const std::string& IR::node::IRVariableDeclStmt::getValue() const noexcept {
    return value;
}

void IR::node::IRVariableDeclStmt::setValue(const std::string& val){
    value = val;
}

Type IR::node::IRVariableDeclStmt::getType() const noexcept {
    return type;
}

void IR::node::IRVariableDeclStmt::setType(Type t) noexcept {
    type = t;
}

bool IR::node::IRVariableDeclStmt::hasAssignExpr() const noexcept {
    return assignExpr != nullptr;
}

bool IR::node::IRVariableDeclStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void IR::node::IRVariableDeclStmt::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}