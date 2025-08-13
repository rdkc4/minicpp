#ifndef IRPRINTF_ST_HPP
#define IRPRINTF_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "defs/IRTree_defs.hpp"

class IRPrintfSt final : public IRStatement {
public:
    IRPrintfSt(IRNodeType ntype);

    const IRExpression* getExp() const noexcept;

    void setExp(std::unique_ptr<IRExpression> _exp, std::unique_ptr<IRTemporary> temp = nullptr);

    const IRTemporary* getTemporaries() const noexcept;

    void setTemporaries(std::unique_ptr<IRTemporary> temp);

    bool hasTemporaries() const noexcept;

    void print(size_t offset) const override;

private:
    std::unique_ptr<IRExpression> exp;
    std::unique_ptr<IRTemporary> temporaries;
};


#endif