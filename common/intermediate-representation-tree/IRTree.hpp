#ifndef IRTREE_HPP
#define IRTREE_HPP

#include <vector>
#include <memory>
#include <optional>

#include "../defs/defs.hpp"
#include "defs/IRTree_defs.hpp"

class IRTree{
    
    public:
        IRTree(IRNodeType nodeType);
        IRTree(IRNodeType nodeType, std::string_view name, std::string_view value, Types type);
        ~IRTree();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // operations related to nodes
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void pushChild(std::unique_ptr<IRTree>&& child);

        void clearChildren();

        IRTree* getChild(size_t index);

        const std::vector<std::unique_ptr<IRTree>>& getChildren() const;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // setters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void setNodeType(IRNodeType type);

        void setName(const std::string& _name);

        void setValue(const std::string& val);

        void setType(Types _type);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // getters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        IRNodeType getNodeType() const;

        const std::string& getName() const;

        const std::string& getValue() const;

        Types getType() const;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // display
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        const std::string toString() const;

        void traverse(size_t offset) const;

    private:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // node type - type of IRT node
        // name - name of a node element (variable name etc.)
        // value - value of a node (literal etc.)
        // type - type of an element (variable, literal etc.)
        // children - subnodes of IRT node
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        IRNodeType nodeType;
        std::string name;
        std::string value;
        std::optional<Types> type;
        std::vector<std::unique_ptr<IRTree>> children;

};

#endif
