#ifndef IRASSIGN_ST_HPP
#define IRASSIGN_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "IRId.hpp"
#include "defs/IRTree_defs.hpp"

class IRAssignSt final : public IRStatement {
public:
    IRAssignSt(IRNodeType ntype);

    const IRId* getVariable() const noexcept;

    const IRExpression* getExp() const noexcept;

    const IRTemporary* getTemporaries() const noexcept;

    void setAssignSt(std::unique_ptr<IRId> var, std::unique_ptr<IRExpression> _exp, std::unique_ptr<IRTemporary> temp = nullptr);

    bool hasTemporaries() const noexcept;

    void print(size_t offset) const override;

private:
    std::unique_ptr<IRId> variable;
    std::unique_ptr<IRExpression> exp;
    std::unique_ptr<IRTemporary> temporaries;
};


#endif