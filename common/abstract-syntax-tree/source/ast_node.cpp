#include "../ast_node.hpp"

syntax::ast::ASTNode::ASTNode(const syntax::Token& token, syntax::ast::ASTNodeType ntype) 
    : token(token), nodeType(ntype) {}

syntax::ast::ASTNode::~ASTNode() = default;

const syntax::Token& syntax::ast::ASTNode::getToken() const noexcept {
    return token;
}

syntax::ast::ASTNodeType syntax::ast::ASTNode::getNodeType() const noexcept {
    return nodeType;
}