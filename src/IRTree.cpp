#include "../include/IRTree.hpp"

IRTree::IRTree(IRNodeType nodeType) : nodeType(nodeType){}

IRTree::IRTree(IRNodeType nodeType, std::string name, std::string value, Types type)
    : nodeType(nodeType), name(name), value(value), type(type){}

IRTree::~IRTree() = default;

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ADD CHILD TO IR TREE 
//-----------------------------------------------------------------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN CHILD AT GIVEN INDEX
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<IRTree> IRTree::getChild(size_t index){
    if(index < children.size()){
        return children.at(index);
    }
    return nullptr;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN ALL CHILDREN
//-----------------------------------------------------------------------------------------------------------------------------------------------------
const std::vector<std::shared_ptr<IRTree>>& IRTree::getChildren() const{
    return children;
}

IRNodeType IRTree::getNodeType() const{
    return nodeType;
}

std::string IRTree::getName() const{
    return name;
}

std::string IRTree::getValue() const{
    return value;
}

std::optional<Types> IRTree::getType() const{
    return type;
}

std::string IRTree::toString(){
    std::string _name = !name.empty() ? " name: " + name : "";
    std::string _value = !value.empty() ? " value: " + value : "";
    return iNodeToString.at(nodeType) + _name + _value + '\n';
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DISPLAY IR TREE (debbuging purposes)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void IRTree::traverse(size_t offset){
    std::cout << std::string(offset*2, ' ') + "|->" + toString();
    for(const auto& child:children){
        child->traverse(offset+1);
    }
}