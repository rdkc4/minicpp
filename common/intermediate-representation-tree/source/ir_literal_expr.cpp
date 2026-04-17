#include "../ir_literal_expr.hpp"

#include "../defs/ir_defs.hpp"

ir::IRLiteralExpr::IRLiteralExpr(const std::string& val, Type type) 
    : IRExpr(ir::IRNodeType::LITERAL, type), value{ val } {}

const std::string& ir::IRLiteralExpr::getValue() const noexcept {
    return value;
}

void ir::IRLiteralExpr::setValue(const std::string& val){
    value = val;
}

void ir::IRLiteralExpr::accept(ir::IRVisitor& visitor){
    visitor.visit(this);
}