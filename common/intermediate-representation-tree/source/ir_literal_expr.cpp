#include "../ir_literal_expr.hpp"

#include "../defs/ir_defs.hpp"

IRLiteralExpr::IRLiteralExpr(const std::string& val, Type type) 
    : IRExpr(IRNodeType::LITERAL, type), value{ val } {}

const std::string& IRLiteralExpr::getValue() const noexcept {
    return value;
}

void IRLiteralExpr::setValue(const std::string& val){
    value = val;
}

void IRLiteralExpr::accept(IRVisitor& visitor){
    visitor.visit(this);
}