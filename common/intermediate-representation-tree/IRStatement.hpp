#ifndef IRSTATEMENT_HPP
#define IRSTATEMENT_HPP

#include "IRNode.hpp"
#include "defs/IRTree_defs.hpp"

class IRStatement : public IRNode {
public:
    IRStatement(IRNodeType ntype);
    
    void print(size_t offset) const override  = 0;
};

#endif