#include "../IRNode.hpp"

IRNode::IRNode(IRNodeType ntype) : nodeType{ ntype } {}

IRNode::~IRNode() = default;

IRNodeType IRNode::getNodeType() const noexcept {
    return nodeType;
}