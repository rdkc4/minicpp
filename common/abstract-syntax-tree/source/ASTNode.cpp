#include "../ASTNode.hpp"

#include <string>
#include <format>

ASTNode::ASTNode(const Token token, ASTNodeType ntype) : token(token), nodeType(ntype) {}
ASTNode::~ASTNode() = default;

const Token& ASTNode::getToken() const noexcept {
    return token;
}

ASTNodeType ASTNode::getNodeType() const noexcept {
    return nodeType;
}

const std::string ASTNode::toString() const{
    return std::format("{} | '{}'\n", astNodeTypeToString.at(nodeType), token.value);
}