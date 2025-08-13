#include "../IRNode.hpp"

#include <format>
#include <string>

IRNode::IRNode(IRNodeType ntype) : nodeType{ ntype } {}

IRNode::~IRNode() = default;

IRNodeType IRNode::getNodeType() const noexcept {
    return nodeType;
}

std::string IRNode::toString() const {
    return std::format("{}\n", irNodeToString.at(nodeType));
}