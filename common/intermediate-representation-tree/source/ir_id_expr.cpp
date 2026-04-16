#include "../ir_id_expr.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRIdExpr::IRIdExpr(std::string_view idName, Type type) 
    : IRExpr(IR::defs::IRNodeType::ID, type), 
      idName{ idName }, 
      value{ "0" } {}

const std::string& IR::node::IRIdExpr::getIdName() const noexcept {
    return idName;
}

void IR::node::IRIdExpr::setIdName(const std::string& name){
    idName = name;
}

const std::string& IR::node::IRIdExpr::getValue() const noexcept {
    return value;
}

void IR::node::IRIdExpr::setValue(const std::string& val){
    value = val;
}

void IR::node::IRIdExpr::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}