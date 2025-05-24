#include "IRTree.hpp"

#include <iostream>
#include <format>

IRTree::IRTree(IRNodeType nodeType) : nodeType{nodeType} {}

IRTree::IRTree(IRNodeType nodeType, std::string_view name, std::string_view value, Types type) : nodeType{nodeType}, name{name}, value{value}, type{type} {}

IRTree::~IRTree() = default;

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ADD CHILD TO IR TREE 
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void IRTree::pushChild(std::unique_ptr<IRTree>&& child){
    children.push_back(std::move(child));
}

void IRTree::clearChildren(){
    children.clear();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN CHILD AT GIVEN INDEX
//-----------------------------------------------------------------------------------------------------------------------------------------------------
IRTree* IRTree::getChild(size_t index){
    if(index < children.size()){
        return children.at(index).get();
    }
    return nullptr;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN ALL CHILDREN
//-----------------------------------------------------------------------------------------------------------------------------------------------------
const std::vector<std::unique_ptr<IRTree>>& IRTree::getChildren() const{
    return children;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// SETTERS
//-----------------------------------------------------------------------------------------------------------------------------------------------------

void IRTree::setNodeType(const IRNodeType type){
    nodeType = type;
}

void IRTree::setName(const std::string& _name){
    name = _name;
}

void IRTree::setValue(const std::string& val){
    value = val;
}

void IRTree::setType(const Types _type){
    type = _type;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// GETTERS
//-----------------------------------------------------------------------------------------------------------------------------------------------------

IRNodeType IRTree::getNodeType() const{
    return nodeType;
}

const std::string& IRTree::getName() const{
    return name;
}

const std::string& IRTree::getValue() const{
    return value;
}

Types IRTree::getType() const{
    return type.has_value() ? type.value() : Types::NO_TYPE;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// PRINT IRT NODE
//-----------------------------------------------------------------------------------------------------------------------------------------------------
const std::string IRTree::toString() const {
    std::string _name = !name.empty() ? " | name: " + name : "";
    std::string _value = !value.empty() ? " | value: " + value : "";
    return std::format("{}{}{}\n", irNodeToString.at(nodeType), _name, _value);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DISPLAY IR TREE (debbuging purposes)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void IRTree::traverse(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    for(const auto& child:children){
        child->traverse(offset+1);
    }
}
