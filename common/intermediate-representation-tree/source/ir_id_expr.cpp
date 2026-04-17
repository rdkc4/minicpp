#include "../ir_id_expr.hpp"

#include "../defs/ir_defs.hpp"

ir::IRIdExpr::IRIdExpr(std::string_view idName, Type type) 
    : IRExpr(ir::IRNodeType::ID, type), 
      idName{ idName }, 
      value{ "0" } {}

const std::string& ir::IRIdExpr::getIdName() const noexcept {
    return idName;
}

void ir::IRIdExpr::setIdName(const std::string& name){
    idName = name;
}

const std::string& ir::IRIdExpr::getValue() const noexcept {
    return value;
}

void ir::IRIdExpr::setValue(const std::string& val){
    value = val;
}

void ir::IRIdExpr::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}