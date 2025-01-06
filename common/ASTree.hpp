#ifndef ASTREE_HPP
#define ASTREE_HPP

#include <vector>
#include <memory>
#include <optional>
#include <queue>

#include "defs.hpp"
#include "token.hpp"

enum class ASTNodeType{
    PROGRAM, FUNCTION_LIST, FUNCTION, PARAMETER, BODY, VARIABLE_LIST, VARIABLE, STATEMENT_LIST, STATEMENT, COMPOUND_STATEMENT, 
    ASSIGNMENT_STATEMENT, RETURN_STATEMENT, IF_STATEMENT, WHILE_STATEMENT, FOR_STATEMENT, DO_WHILE_STATEMENT, SWITCH_STATEMENT, 
    NUMERICAL_EXPRESSION, EXPRESSION, RELATIONAL_EXPRESSION, FUNCTION_CALL, ARGUMENT, LITERAL, ID, CASE, DEFAULT, BREAK
};

extern const std::unordered_map<ASTNodeType, std::string> astNodeTypeToString;

class ASTree{
    public:
    
        ASTree(ASTNodeType nodeType, const Token& token);
        ASTree(ASTNodeType nodeType, const Token& token, Types type);

        ~ASTree();

        void pushChild(std::shared_ptr<ASTree> child);

        std::shared_ptr<ASTree> getChild(size_t index);

        const std::vector<std::shared_ptr<ASTree>>& getChildren() const;

        void setType(Types t);

        ASTNodeType getNodeType() const;

        std::optional<Types> getType() const;

        const std::shared_ptr<Token> getToken() const;

        std::string toString();

        void traverse(size_t offset);

    private:
        ASTNodeType nodeType;
        std::shared_ptr<Token> token;
        std::optional<Types> type;

        std::vector<std::shared_ptr<ASTree>> children;

};

#endif