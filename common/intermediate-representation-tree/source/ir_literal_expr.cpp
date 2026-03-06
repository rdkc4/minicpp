#include "../ir_literal_expr.hpp"

IRLiteralExpr::IRLiteralExpr(IRNodeType ntype, const std::string& val, Type type) : IRExpr(ntype, type), value{ val } {}

const std::string& IRLiteralExpr::getValue() const noexcept {
    return value;
}

void IRLiteralExpr::setValue(const std::string& val){
    value = val;
}