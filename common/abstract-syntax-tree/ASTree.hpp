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

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // node operations
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void pushChild(std::unique_ptr<ASTree>&& child);

        ASTree* getChild(size_t index) const noexcept;

        const std::vector<std::unique_ptr<ASTree>>& getChildren() const noexcept;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // setters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void setType(const Types t) noexcept;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // getters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        Types getType() const noexcept;

        ASTNodeType getNodeType() const noexcept;

        const Token& getToken() const noexcept;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // display
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::string toString() const;

        void traverse(size_t offset) const;

    private:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // token - token related to AST node
        // type - type of a node (optional, storing types of variables etc.)
        // node type - type of a AST node
        // children - AST subnodes of a node
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        Token token;
        std::optional<Types> type;
        ASTNodeType nodeType;
        std::vector<std::unique_ptr<ASTree>> children;

};

#endif
