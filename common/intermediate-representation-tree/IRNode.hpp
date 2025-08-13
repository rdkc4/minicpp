#ifndef IRNODE_HPP
#define IRNODE_HPP

#include <string>

#include "defs/IRTree_defs.hpp"

class IRNode {
public:
    IRNode(IRNodeType ntype);
    virtual ~IRNode();

    IRNodeType getNodeType() const noexcept;

    std::string toString() const;

    virtual void print(size_t offset) const = 0;

private:
    IRNodeType nodeType;
};

#endif