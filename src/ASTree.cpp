#include "../include/ASTree.hpp"

ASTree::ASTree(ASTNodeType nodeType, const Token& token) : nodeType(nodeType), token(std::make_shared<Token>(token)){}
ASTree::ASTree(ASTNodeType nodeType, const Token& token, Types type) : nodeType(nodeType), token(std::make_shared<Token>(token)), type(type){}

ASTree::~ASTree() = default;

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// ADD CHILD TO AST TREE
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void ASTree::pushChild(std::shared_ptr<ASTree> child){
    children.push_back(child);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN CHILD AT GIVEN INDEX
//-----------------------------------------------------------------------------------------------------------------------------------------------------
std::shared_ptr<ASTree> ASTree::getChild(size_t index){
    if(index < children.size()){
        return children.at(index);
    }
    return nullptr;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// RETURN ALL CHILDREN
//-----------------------------------------------------------------------------------------------------------------------------------------------------
const std::vector<std::shared_ptr<ASTree>>& ASTree::getChildren() const{
    return children;
}

void ASTree::setType(Types t){
    type = t;
}

ASTNodeType ASTree::getNodeType() const{
    return nodeType;
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

//-----------------------------------------------------------------------------------------------------------------------------------------------------
// DISPLAY AST TREE (debbuging purposes)
//-----------------------------------------------------------------------------------------------------------------------------------------------------
void ASTree::traverse(size_t offset){
    std::cout << std::string(offset*2, ' ') + "|->" + toString();
    for(const auto& child:children){
        child->traverse(offset+1);
    }

}
