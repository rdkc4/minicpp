#include "../ASTLiteral.hpp"

#include <string>
#include <iostream>
#include <format>

ASTLiteral::ASTLiteral(const Token token, ASTNodeType ntype, Types type) : ASTExpression(token, ntype, type) {}

void ASTLiteral::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
}