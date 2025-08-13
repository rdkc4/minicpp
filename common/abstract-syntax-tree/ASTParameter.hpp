#ifndef ASTPARAMETER_HPP
#define ASTPARAMETER_HPP

#include "ASTNode.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

class ASTParameter final : public ASTNode {
public:
    ASTParameter(const Token& token, ASTNodeType ntype, Types type = Types::NO_TYPE);
    
    Types getType() const noexcept;

    void setType(Types t) noexcept;

    void print(size_t offset) const override;

private:
    Types type;
};


#endif