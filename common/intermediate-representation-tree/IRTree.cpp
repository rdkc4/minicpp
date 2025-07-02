#include "IRTree.hpp"

#include <iostream>
#include <format>

IRTree::IRTree(IRNodeType nodeType) : nodeType{nodeType} {}

IRTree::IRTree(std::string_view name, std::string_view value, Types type, IRNodeType nodeType) 
    : name{ name }, value{ value }, type{ type }, nodeType{ nodeType } {}

IRTree::~IRTree() = default;

void IRTree::pushChild(std::unique_ptr<IRTree>&& child){
    children.push_back(std::move(child));
}

void IRTree::setChild(std::unique_ptr<IRTree> child, size_t pos){
    if(pos < children.size()){
        children[pos] = std::move(child);
    }
}

void IRTree::resizeChildren(size_t n){
    children.resize(n);
}

void IRTree::eraseChildren(size_t startIndex) {
    if(startIndex < children.size()){
        children.erase(children.begin() + startIndex, children.end());
    }
}

void IRTree::clearChildren() noexcept {
    children.clear();
}

IRTree* IRTree::getChild(size_t index) const noexcept {
    if(index < children.size()){
        return children[index].get();
    }
    return nullptr;
}

const std::vector<std::unique_ptr<IRTree>>& IRTree::getChildren() const noexcept {
    return children;
}

void IRTree::setNodeType(const IRNodeType type) noexcept {
    nodeType = type;
}

void IRTree::setName(const std::string& _name){
    name = _name;
}

void IRTree::setValue(const std::string& val){
    value = val;
}

void IRTree::setType(const Types _type) noexcept {
    type = _type;
}

IRNodeType IRTree::getNodeType() const noexcept {
    return nodeType;
}

const std::string& IRTree::getName() const noexcept {
    return name;
}

const std::string& IRTree::getValue() const noexcept {
    return value;
}

Types IRTree::getType() const noexcept {
    return type.value_or(Types::NO_TYPE);
}

std::string IRTree::toString() const {
    std::string _name = !name.empty() ? " | name: " + name : "";
    std::string _value = !value.empty() ? " | value: " + value : "";
    return std::format("{}{}{}\n", irNodeToString.at(nodeType), _name, _value);
}

void IRTree::traverse(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    for(const auto& child:children){
        child->traverse(offset+1);
    }
}
