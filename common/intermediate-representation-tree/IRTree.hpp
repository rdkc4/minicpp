#ifndef IRTREE_HPP
#define IRTREE_HPP

#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>

#include "../defs/defs.hpp"

enum class IRNodeType{
    PROGRAM, FUNCTION, PARAMETER, VARIABLE, ARGUMENT, ID, LITERAL, IF, WHILE, FOR, DO_WHILE, SWITCH, CASE, 
    DEFAULT, BREAK, ASSIGN, COMPOUND, CALL, RETURN, ADD, SUB, DIV, MUL, CMP, AND, OR, XOR
};

extern const std::unordered_set<IRNodeType> irOperators;

extern const std::unordered_map<IRNodeType, std::string> irNodeToString;

extern const std::unordered_map<std::string, IRNodeType> stringToArop;

extern const std::unordered_map<std::string, IRNodeType> stringToBitop;

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