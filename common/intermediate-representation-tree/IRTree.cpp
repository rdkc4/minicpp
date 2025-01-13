#include "IRTree.hpp"

#include <iostream>
#include <format>

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

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SETTERS
//-----------------------------------------------------------------------------------------------------------------------------------------------------

void IRTree::setNodeType(IRNodeType type){
    nodeType = type;
}

void IRTree::setName(std::string& _name){
    name = _name;
}

void IRTree::setValue(std::string& val){
    value = val;
}

void IRTree::setType(Types _type){
    type = _type;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// GETTERS
//-----------------------------------------------------------------------------------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// PRINT IRT NODE
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::string IRTree::toString(){
    std::string _name = !name.empty() ? " | name: " + name : "";
    std::string _value = !value.empty() ? " | value: " + value : "";
    return std::format("{}{}{}\n", irNodeToString.at(nodeType), _name, _value);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DISPLAY IR TREE (debbuging purposes)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void IRTree::traverse(size_t offset){
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    for(const auto& child:children){
        child->traverse(offset+1);
    }
}
