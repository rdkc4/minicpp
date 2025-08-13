#ifndef ASTID_HPP
#define ASTID_HPP

#include "ASTExpression.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

class ASTId final : public ASTExpression {
public:
    ASTId(const Token token, ASTNodeType ntype, Types type = Types::NO_TYPE);
    
    void print(size_t offset) const override;
};


#endif