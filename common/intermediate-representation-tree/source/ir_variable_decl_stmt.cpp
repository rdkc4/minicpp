#include "../ir_variable_decl_stmt.hpp"

#include "../defs/ir_defs.hpp"

ir::IRVariableDeclStmt::IRVariableDeclStmt(std::string_view varName, Type type) 
    : IRStmt(ir::IRNodeType::VARIABLE), 
      varName{ varName }, 
      value{ "0" }, 
      type{ type } {}

void ir::IRVariableDeclStmt::setAssignExpr(
    std::unique_ptr<IRExpr> expr, 
    std::unique_ptr<IRTemporaryExpr> tempExpr
){
    assignExpr = std::move(expr);
    temporaryExpr = std::move(tempExpr);
}

const std::string& ir::IRVariableDeclStmt::getVarName() const noexcept {
    return varName;
}

void ir::IRVariableDeclStmt::setVarName(const std::string& var){
    varName = var;
}

const std::string& ir::IRVariableDeclStmt::getValue() const noexcept {
    return value;
}

void ir::IRVariableDeclStmt::setValue(const std::string& val){
    value = val;
}

Type ir::IRVariableDeclStmt::getType() const noexcept {
    return type;
}

void ir::IRVariableDeclStmt::setType(Type t) noexcept {
    type = t;
}

bool ir::IRVariableDeclStmt::hasAssignExpr() const noexcept {
    return assignExpr != nullptr;
}

bool ir::IRVariableDeclStmt::hasTemporaryExpr() const noexcept {
    return temporaryExpr != nullptr;
}

void ir::IRVariableDeclStmt::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}