#ifndef ASTSTATEMENT_HPP
#define ASTSTATEMENT_HPP

#include "ASTNode.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

class ASTStatement : public ASTNode {
public:
    ASTStatement(const Token& token, ASTNodeType ntype);

    void print(size_t offset) const override = 0;
};


#endif