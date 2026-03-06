#include "../ASTLiteral.hpp"

ASTLiteral::ASTLiteral(const Token token, ASTNodeType ntype, Types type) : ASTExpression(token, ntype, type) {}