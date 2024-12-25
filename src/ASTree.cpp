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

void ASTree::toString(){
    std::cout <<"Node Type: " + nodeTypeToString.at(nodeType) + ", Token: " + token->value + " | ";
}

void ASTree::traverse(){
    std::queue<std::shared_ptr<ASTree>> q;
    q.push(std::make_shared<ASTree>(*this));

    while(!q.empty()){
        int sz = q.size();
        for(int i = 0; i < sz; i++){
            auto curr = q.front();
            std::cout << "parent\n";
            curr->toString();
            q.pop();
            for(const auto& child: curr->getChildren()){
                q.push(child);
                child->toString();
            }
            std::cout << "\n";
        }
        std::cout << "--------------------\n";
    }
}