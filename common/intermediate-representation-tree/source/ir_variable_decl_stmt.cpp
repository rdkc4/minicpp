#include "../ir_variable_decl_stmt.hpp"

IRVariableDeclStmt::IRVariableDeclStmt(IRNodeType ntype, std::string_view varName, Type type) 
    : IRStmt(ntype), varName{ varName }, value{ "0" }, type{ type } {}

void IRVariableDeclStmt::setAssign(std::unique_ptr<IRExpr> assign, std::unique_ptr<IRTemporaryExpr> temp){
    assignment = std::move(assign);
    temporaries = std::move(temp);
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

bool IRVariableDeclStmt::hasAssign() const noexcept {
    return assignment != nullptr;
}

bool IRVariableDeclStmt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}

void IRVariableDeclStmt::accept(IRVisitor& visitor){
    visitor.visit(this);
}