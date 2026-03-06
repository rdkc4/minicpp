#include "../ir_variable_decl_stmt.hpp"

IRVariableDeclStmt::IRVariableDeclStmt(IRNodeType ntype, std::string_view varName, Type type) : IRStmt(ntype), varName{ varName }, value{ "0" }, type{ type } {}

const IRExpr* IRVariableDeclStmt::getAssign() const noexcept {
    return assignment.get();
}

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

const IRTemporaryExpr* IRVariableDeclStmt::getTemporaries() const noexcept {
    return temporaries.get();
}

bool IRVariableDeclStmt::hasTemporaries() const noexcept {
    return temporaries != nullptr;
}