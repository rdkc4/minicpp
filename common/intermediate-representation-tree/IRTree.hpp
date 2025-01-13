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
        IRTree(IRNodeType nodeType, std::string name, std::string value, Types type);
        ~IRTree();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // operations related to nodes
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void pushChild(std::shared_ptr<IRTree> child);

        void clearChildren();

        std::shared_ptr<IRTree> getChild(size_t index);

        const std::vector<std::shared_ptr<IRTree>>& getChildren() const;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // setters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void setNodeType(IRNodeType type);

        void setName(std::string& _name);

        void setValue(std::string& val);

        void setType(Types _type);

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // getters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        IRNodeType getNodeType() const;

        std::string getName() const;

        std::string getValue() const;

        std::optional<Types> getType() const;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // display
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::string toString();

        void traverse(size_t offset);

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
        std::vector<std::shared_ptr<IRTree>> children;

};

#endif
