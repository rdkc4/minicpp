#include "../ir_node.hpp"

IRNode::IRNode(IRNodeType ntype) : nodeType{ ntype } {}

IRNode::~IRNode() = default;

IRNodeType IRNode::getNodeType() const noexcept {
    return nodeType;
}