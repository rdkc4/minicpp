#include "../ASTNode.hpp"

ASTNode::ASTNode(const Token token, ASTNodeType ntype) : token(token), nodeType(ntype) {}

ASTNode::~ASTNode() = default;

const Token& ASTNode::getToken() const noexcept {
    return token;
}

ASTNodeType ASTNode::getNodeType() const noexcept {
    return nodeType;
}