#include "../include/ASTree.hpp"

ASTree::ASTree(ASTNodeType nodeType, const Token& token) : nodeType(nodeType), token(std::make_shared<Token>(token)){}
ASTree::ASTree(ASTNodeType nodeType, const Token& token, Types type) : nodeType(nodeType), token(std::make_shared<Token>(token)), type(type){}

ASTree::~ASTree() = default;

void ASTree::pushChild(std::shared_ptr<ASTree> child){
    children.push_back(child);
}

std::shared_ptr<ASTree> ASTree::getChild(size_t index){
    if(index < children.size()){
        return children.at(index);
    }
    return nullptr;
}

const std::vector<std::shared_ptr<ASTree>>& ASTree::getChildren() const{
    return children;
}

void ASTree::setType(Types t){
    type = t;
}

std::optional<Types> ASTree::getType() const{
    return type;
}

const std::shared_ptr<Token> ASTree::getToken() const {
    return token;
}

std::string ASTree::toString(){
    return "|" + nodeTypeToString.at(nodeType) + " | " + token->value + "\n";
}

void ASTree::traverse(size_t offset){
    std::cout << std::string(offset*2, ' ') + "|->" + toString();
    for(const auto& child:children){
        child->traverse(offset+1);
    }

}
