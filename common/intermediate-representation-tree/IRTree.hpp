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

        // children modifiers
        void pushChild(std::unique_ptr<IRTree>&& child);
        void clearChildren() noexcept;
        
        // children getters
        IRTree* getChild(size_t index) const noexcept;
        const std::vector<std::unique_ptr<IRTree>>& getChildren() const noexcept;

        // setters
        void setNodeType(IRNodeType type) noexcept;
        void setName(const std::string& _name);
        void setValue(const std::string& val);
        void setType(Types _type) noexcept;

        // getters
        IRNodeType getNodeType() const noexcept;
        const std::string& getName() const noexcept;
        const std::string& getValue() const noexcept;
        Types getType() const noexcept;

        // display ir tree
        std::string toString() const;
        void traverse(size_t offset) const;

    private:
        // name of a node (variable name etc.)
        std::string name;
        // value of a node
        std::string value;
        // type of an element (optional, types of a variables etc.)
        std::optional<Types> type;
        // type of a IRT node
        IRNodeType nodeType;
        // children of the node
        std::vector<std::unique_ptr<IRTree>> children;

};

#endif
