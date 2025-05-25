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
        IRTree(std::string_view name, std::string_view value, Types type, IRNodeType nodeType);
        ~IRTree();

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // operations related to nodes
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void pushChild(std::unique_ptr<IRTree>&& child);

        void clearChildren() noexcept;

        IRTree* getChild(size_t index) const noexcept;

        const std::vector<std::unique_ptr<IRTree>>& getChildren() const noexcept;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // setters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        void setNodeType(IRNodeType type) noexcept;

        void setName(const std::string& _name);

        void setValue(const std::string& val);

        void setType(Types _type) noexcept;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // getters
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        IRNodeType getNodeType() const noexcept;

        const std::string& getName() const noexcept;

        const std::string& getValue() const noexcept;

        Types getType() const noexcept;

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // display
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::string toString() const;

        void traverse(size_t offset) const;

    private:

        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        // name - name of a node element (variable name etc.)
        // value - value of a node (literal etc.)
        // type - type of an element (variable, literal etc.)
        // node type - type of IRT node
        // children - subnodes of IRT node
        //-----------------------------------------------------------------------------------------------------------------------------------------------------
        std::string name;
        std::string value;
        std::optional<Types> type;
        IRNodeType nodeType;
        std::vector<std::unique_ptr<IRTree>> children;

};

#endif
