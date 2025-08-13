#ifndef ASTNODE_HPP
#define ASTNODE_HPP

#include <string>

#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

class ASTNode{ 
public:
    ASTNode(const Token token, ASTNodeType ntype);
    virtual ~ASTNode();

    const Token& getToken() const noexcept;

    ASTNodeType getNodeType() const noexcept;

    const std::string toString() const;

    virtual void print(size_t offset) const = 0;

private:
    Token token;
    ASTNodeType nodeType;
};


#endif