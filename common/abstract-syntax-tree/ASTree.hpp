#ifndef ASTREE_HPP
#define ASTREE_HPP

#include <vector>
#include <memory>
#include <optional>

#include "../defs/defs.hpp"
#include "../token/token.hpp"
#include "defs/ASTree_defs.hpp"

class ASTree{
    public:
    
        ASTree(const Token& token, ASTNodeType nodeType);
        ASTree(const Token& token, Types type, ASTNodeType nodeType);

        ~ASTree();

        // children modifiers
        void pushChild(std::unique_ptr<ASTree>&& child);

        // children getters
        ASTree* getChild(size_t index) const noexcept;
        const std::vector<std::unique_ptr<ASTree>>& getChildren() const noexcept;

        // setters
        void setType(const Types t) noexcept;

        // getters
        Types getType() const noexcept;
        ASTNodeType getNodeType() const noexcept;
        const Token& getToken() const noexcept;

        // display ast
        std::string toString() const;
        void traverse(size_t offset) const;

    private:
        // token related to ast node
        Token token;
        // type of a node (optional, types of variables etc.)
        std::optional<Types> type;
        // type of a ast node
        ASTNodeType nodeType;
        // children of the node
        std::vector<std::unique_ptr<ASTree>> children;

};

#endif
