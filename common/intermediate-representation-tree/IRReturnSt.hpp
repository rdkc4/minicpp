#ifndef IRRETURN_ST_HPP
#define IRRETURN_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "defs/IRTree_defs.hpp"

class IRReturnSt final : public IRStatement {
public:
    IRReturnSt(IRNodeType ntype);

    const IRExpression* getExp() const noexcept;

    void setExp(std::unique_ptr<IRExpression> _exp, std::unique_ptr<IRTemporary> temp = nullptr);

    const IRTemporary* getTemporaries() const noexcept;

    bool returns() const noexcept;

    bool hasTemporaries() const noexcept;

    void print(size_t offset) const override;

private:
    std::unique_ptr<IRExpression> exp;
    std::unique_ptr<IRTemporary> temporaries;
};


#endif