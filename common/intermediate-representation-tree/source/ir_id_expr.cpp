#include "../ir_id_expr.hpp"

#include "../defs/ir_defs.hpp"

IRIdExpr::IRIdExpr(std::string_view idName, Type type) : IRExpr(IRNodeType::ID, type), idName{ idName }, value{ "0" } {}

const std::string& IRIdExpr::getIdName() const noexcept {
    return idName;
}

void IRIdExpr::setIdName(const std::string& name){
    idName = name;
}

const std::string& IRIdExpr::getValue() const noexcept {
    return value;
}

void IRIdExpr::setValue(const std::string& val){
    value = val;
}

void IRIdExpr::accept(IRVisitor& visitor){
    visitor.visit(this);
}