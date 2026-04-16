#include "../ir_literal_expr.hpp"

#include "../defs/ir_defs.hpp"

IR::node::IRLiteralExpr::IRLiteralExpr(const std::string& val, Type type) 
    : IRExpr(IR::defs::IRNodeType::LITERAL, type), value{ val } {}

const std::string& IR::node::IRLiteralExpr::getValue() const noexcept {
    return value;
}

void IR::node::IRLiteralExpr::setValue(const std::string& val){
    value = val;
}

void IR::node::IRLiteralExpr::accept(IR::visitor::IRVisitor& visitor){
    visitor.visit(this);
}