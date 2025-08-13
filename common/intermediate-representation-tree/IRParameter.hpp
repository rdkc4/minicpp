#ifndef IRPARAMETER_HPP
#define IRPARAMETER_HPP

#include <string>
#include <string_view>

#include "IRNode.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

class IRParameter final : public IRNode {
public:
    IRParameter(IRNodeType ntype, std::string_view parName, Types type);

    const std::string& getParName() const noexcept;

    void setParName(const std::string& _parName);

    Types getType() const noexcept;

    void setType(Types t) noexcept;

    void print(size_t offset) const override;

private:
    std::string parName;
    Types type;
};


#endif