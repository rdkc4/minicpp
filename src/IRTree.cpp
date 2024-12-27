#include "../include/IRTree.hpp"

IRTree::IRTree(IRNodeType nodeType) : nodeType(nodeType){}

IRTree::IRTree(IRNodeType nodeType, std::string name, std::string value, Types type)
    : nodeType(nodeType), name(name), value(value), type(type){}

IRTree::~IRTree() = default;

void IRTree::pushChild(std::shared_ptr<IRTree> child){
    children.push_back(child);
}

void IRTree::clearChildren(){
    children.clear();
}

void IRTree::setNodeType(IRNodeType type){
    nodeType = type;
}

void IRTree::setName(std::string& newName){
    name = newName;
}

void IRTree::setValue(std::string& val){
    value = val;
}

void IRTree::setType(Types newType){
    type = newType;
}

std::shared_ptr<IRTree> IRTree::getChild(size_t index){
    if(index < children.size()){
        return children.at(index);
    }
    return nullptr;
}

const std::vector<std::shared_ptr<IRTree>>& IRTree::getChildren() const{
    return children;
}

IRNodeType IRTree::getNodeType() const{
    return nodeType;
}

std::optional<std::string> IRTree::getName() const{
    return value;
}

std::optional<std::string> IRTree::getValue() const{
    return value;
}

std::optional<Types> IRTree::getType() const{
    return type;
}

std::string IRTree::toString(){
    return iNodeToString.at(nodeType) + " n:" + name + " v:" + value + '\n';
}

void IRTree::traverse(size_t offset){
    std::cout << std::string(offset*2, ' ') + "|->" + toString();
    for(const auto& child:children){
        child->traverse(offset+1);
    }
}