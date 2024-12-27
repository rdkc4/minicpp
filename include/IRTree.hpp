#ifndef IRTREE_HPP
#define IRTREE_HPP

#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>

#include "defs.hpp"

enum class IRNodeType{PROGRAM, FUNCTION, PARAMETER, VARIABLE, LITERAL, IF, ASSIGN, COMPOUND, CALL, RETURN, ADD, SUB, CMP};

const std::unordered_map<IRNodeType, std::string> iNodeToString = {
    {IRNodeType::PROGRAM, "PROGRAM"},
    {IRNodeType::FUNCTION, "FUNCTION"},
    {IRNodeType::PARAMETER, "PARAMETER"},
    {IRNodeType::VARIABLE, "VARIABLE"},
    {IRNodeType::LITERAL, "LITERAL"},
    {IRNodeType::IF, "IF"},
    {IRNodeType::ASSIGN, "ASSIGN"},
    {IRNodeType::COMPOUND, "COMPOUND"},
    {IRNodeType::CALL, "CALL"},
    {IRNodeType::RETURN, "RETURN"},
    {IRNodeType::ASSIGN, "ASSIGN"},
    {IRNodeType::ADD, "ADD"},
    {IRNodeType::SUB, "SUB"},
    {IRNodeType::CMP, "CMP"}
};

const std::unordered_map<std::string, IRNodeType> stringToArop = {
    {"+", IRNodeType::ADD},
    {"-", IRNodeType::SUB}
};

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

        std::optional<std::string> getName() const;

        std::optional<std::string> getValue() const;

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