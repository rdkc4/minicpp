#include "ASTree.hpp"

#include <iostream>
#include <format>

ASTree::ASTree(const Token& token, ASTNodeType nodeType) : token{ token }, nodeType{ nodeType } {}

ASTree::ASTree(const Token& token, Types type, ASTNodeType nodeType) : token{ token }, type{ type}, nodeType{ nodeType } {}

ASTree::~ASTree() = default;

void ASTree::pushChild(std::unique_ptr<ASTree>&& child){
    children.push_back(std::move(child));
}

ASTree* ASTree::getChild(size_t index) const noexcept {
    if(index < children.size()){
        return children[index].get();
    }
    return nullptr;
}

const std::vector<std::unique_ptr<ASTree>>& ASTree::getChildren() const noexcept {
    return children;
}

void ASTree::setType(const Types t) noexcept {
    type = t;
}

Types ASTree::getType() const noexcept {
    return type.value_or(Types::NO_TYPE);
}

ASTNodeType ASTree::getNodeType() const noexcept {
    return nodeType;
}

const Token& ASTree::getToken() const noexcept {
    return token;
}

std::string ASTree::toString() const {
    return std::format("{} | '{}'\n", astNodeTypeToString.at(nodeType), token.value);
}

void ASTree::traverse(size_t offset) const {
    std::cout << std::format("{}|-> {}", std::string(offset*2, ' '), toString());
    for(const auto& child:children){
        child->traverse(offset+1);
    }

}
