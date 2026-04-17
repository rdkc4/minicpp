#include "../ir_node.hpp"

ir::IRNode::IRNode(ir::IRNodeType ntype) 
    : nodeType{ ntype } {}

ir::IRNodeType ir::IRNode::getNodeType() const noexcept {
    return nodeType;
}