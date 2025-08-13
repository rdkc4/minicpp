#ifndef IREXPRESSION_HPP
#define IREXPRESSION_HPP

#include "IRNode.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

class IRExpression : public IRNode {
public:
    IRExpression(IRNodeType ntype, Types type);

    Types getType() const noexcept;

    void setType(Types t) noexcept;

    void print(size_t offset) const override = 0;

private:
    Types type;
};

#endif