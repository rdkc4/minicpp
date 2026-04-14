#include "../ir_node.hpp"

IR::node::IRNode::IRNode(IR::defs::IRNodeType ntype) : nodeType{ ntype } {}

IR::node::IRNode::~IRNode() = default;

IR::defs::IRNodeType IR::node::IRNode::getNodeType() const noexcept {
    return nodeType;
}