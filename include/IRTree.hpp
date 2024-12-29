#ifndef IRTREE_HPP
#define IRTREE_HPP

#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>

#include "defs.hpp"

class IRTree{
    
    public:
        IRTree(IRNodeType nodeType);
        IRTree(IRNodeType nodeType, std::string name, std::string value, Types type);
        ~IRTree();

        void pushChild(std::shared_ptr<IRTree> child);

        void clearChildren();

        void setNodeType(IRNodeType type);

        void setName(std::string& newName);

        void setValue(std::string& val);

        void setType(Types newType);

        std::shared_ptr<IRTree> getChild(size_t index);

        const std::vector<std::shared_ptr<IRTree>>& getChildren() const;

        IRNodeType getNodeType() const;

        std::string getName() const;

        std::string getValue() const;

        std::optional<Types> getType() const;

        std::string toString();

        void traverse(size_t offset);

    private:

        IRNodeType nodeType;
        std::string name;
        std::string value;
        std::optional<Types> type;
        std::vector<std::shared_ptr<IRTree>> children;

};

#endif