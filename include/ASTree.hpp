#ifndef ASTREE_HPP
#define ASTREE_HPP

#include <vector>
#include <memory>
#include <optional>
#include <queue>
#include "symbol.hpp"

class ASTree{
    public:
    
        ASTree(ASTNodeType nodeType, const Token& token) : nodeType(nodeType), token(std::make_shared<Token>(token)){}
        ASTree(ASTNodeType nodeType, const Token& token, Types type) : nodeType(nodeType), token(std::make_shared<Token>(token)), type(type){}

        ~ASTree() = default;

        void pushChild(std::shared_ptr<ASTree> child){
            children.push_back(child);
        }

        std::shared_ptr<ASTree> getChild(size_t index){
            if(index < children.size()){
                return children.at(index);
            }
            return nullptr;
        }

        const std::vector<std::shared_ptr<ASTree>>& getChildren() const{
            return children;
        }

        void setType(Types t){
            type = t;
        }

        std::optional<Types> getType() const{
            return type;
        }

        const std::shared_ptr<Token> getToken() const {
            return token;
        }

        void toString(){
            std::cout <<"Node Type: " + nodeTypeToString.at(nodeType) + ", Token: " + token->value + "\n";
        }

        void traverse(){
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
                }
                std::cout << "--------------------\n";
            }
        }

    private:
        ASTNodeType nodeType;
        std::shared_ptr<Token> token;
        std::optional<Types> type;

        std::vector<std::shared_ptr<ASTree>> children;


};


#endif