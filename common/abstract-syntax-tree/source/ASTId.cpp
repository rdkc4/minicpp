#include "../ASTId.hpp"

#include <string>
#include <iostream>
#include <format>

ASTId::ASTId(const Token token, ASTNodeType ntype, Types type) : ASTExpression(token, ntype, type) {}
    
void ASTId::print(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset * 2, ' '), toString());
}