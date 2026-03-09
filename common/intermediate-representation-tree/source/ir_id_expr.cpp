#include "../ir_id_expr.hpp"

IRIdExpr::IRIdExpr(IRNodeType ntype, std::string_view idName, Type type) : IRExpr(ntype, type), idName{ idName }, value{ "0" } {}

const std::string& IRIdExpr::getIdName() const noexcept {
    return idName;
}

void IRIdExpr::setIdName(const std::string& _idName){
    idName = _idName;
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