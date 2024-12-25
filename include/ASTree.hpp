#ifndef ASTREE_HPP
#define ASTREE_HPP

#include <vector>
#include <memory>
#include <optional>

#include "symbol.hpp"

class ASTree{
    public:
    
        ASTree(ASTNodeType nodeType, const Token& token) : nodeType(nodeType), token(std::make_shared<Token>(token)){}
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

    private:
        ASTNodeType nodeType;
        std::optional<Types> type;
        std::shared_ptr<Token> token;

        std::vector<std::shared_ptr<ASTree>> children;


};


#endif