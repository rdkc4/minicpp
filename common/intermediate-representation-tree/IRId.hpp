#ifndef IRID_HPP
#define IRID_HPP

#include <string>
#include <string_view>

#include "IRExpression.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

class IRId final : public IRExpression {
public:
    IRId(IRNodeType ntype, std::string_view idName, Types type);

    const std::string& getIdName() const noexcept;

    void setIdName(const std::string& _idName);

    const std::string& getValue() const noexcept;

    void setValue(const std::string& val);

    void print(size_t offset) const override;

private:
    std::string idName;
    std::string value;
};

#endif