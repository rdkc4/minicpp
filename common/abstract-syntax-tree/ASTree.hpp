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
    
        ASTree(ASTNodeType nodeType, const Token& token);
        ASTree(ASTNodeType nodeType, const Token& token, Types type);

        ~ASTree();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // node operations
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void pushChild(std::unique_ptr<ASTree>&& child);

        ASTree* getChild(size_t index);

        const std::vector<std::unique_ptr<ASTree>>& getChildren() const;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // setters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void setType(Types t);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // getters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::optional<Types> getType() const;

        ASTNodeType getNodeType() const;

        const Token getToken() const;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // display
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::string toString();

        void traverse(size_t offset);

    private:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // node type - type of a AST node
        // token - token related to AST node
        // type - type of a node (optional, storing types of variables etc.)
        // children - AST subnodes of a node
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        ASTNodeType nodeType;
        Token token;
        std::optional<Types> type;
        std::vector<std::unique_ptr<ASTree>> children;

};

#endif
