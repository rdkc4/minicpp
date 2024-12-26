#ifndef ASTREE_HPP
#define ASTREE_HPP

#include <vector>
#include <memory>
#include <optional>
#include <queue>
#include "symbol.hpp"

class ASTree{
    public:
    
        ASTree(ASTNodeType nodeType, const Token& token);
        ASTree(ASTNodeType nodeType, const Token& token, Types type);

        ~ASTree();

        void pushChild(std::shared_ptr<ASTree> child);

        std::shared_ptr<ASTree> getChild(size_t index);

        const std::vector<std::shared_ptr<ASTree>>& getChildren() const;

        void setType(Types t);

        ASTNodeType getNodeType() const;

        std::optional<Types> getType() const;

        const std::shared_ptr<Token> getToken() const;

        std::string toString();

        void traverse(size_t offset);

    private:
        ASTNodeType nodeType;
        std::shared_ptr<Token> token;
        std::optional<Types> type;

        std::vector<std::shared_ptr<ASTree>> children;

};


#endif