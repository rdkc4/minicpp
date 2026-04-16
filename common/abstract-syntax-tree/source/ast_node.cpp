#include "../ast_node.hpp"

AST::node::ASTNode::ASTNode(const Token token, AST::defs::ASTNodeType ntype) 
    : token(token), nodeType(ntype) {}

AST::node::ASTNode::~ASTNode() = default;

const Token& AST::node::ASTNode::getToken() const noexcept {
    return token;
}

AST::defs::ASTNodeType AST::node::ASTNode::getNodeType() const noexcept {
    return nodeType;
}