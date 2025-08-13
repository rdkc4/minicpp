#ifndef IRLITERAL_HPP
#define IRLITERAL_HPP

#include <string>

#include "IRExpression.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

class IRLiteral final : public IRExpression {
public:
    IRLiteral(IRNodeType ntype, const std::string& val, Types type);

    const std::string& getValue() const noexcept;

    void setValue(const std::string& val);

    void print(size_t offset) const override;

private:
    std::string value;
};

#endif