#ifndef IRVARIABLE_HPP
#define IRVARIABLE_HPP

#include <memory>
#include <string>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

class IRVariable final : public IRStatement {
public:
    IRVariable(IRNodeType ntype, std::string_view varName, Types type);

    const IRExpression* getAssign() const noexcept;

    void setAssign(std::unique_ptr<IRExpression> assign, std::unique_ptr<IRTemporary> temp = nullptr);

    const std::string& getVarName() const noexcept;

    void setVarName(const std::string& var);

    const std::string& getValue() const noexcept;

    void setValue(const std::string& val);

    Types getType() const noexcept;

    void setType(Types t) noexcept;

    bool hasAssign() const noexcept;

    const IRTemporary* getTemporaries() const noexcept;

    bool hasTemporaries() const noexcept;

    void print(size_t offset) const override;

private:
    std::string varName;
    std::string value;
    Types type;
    std::unique_ptr<IRExpression> assignment;
    std::unique_ptr<IRTemporary> temporaries;
};


#endif