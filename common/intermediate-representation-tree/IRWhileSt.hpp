#ifndef IRWHILE_ST_HPP
#define IRWHILE_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "defs/IRTree_defs.hpp"

class IRWhileSt final : public IRStatement {
public:
    IRWhileSt(IRNodeType ntype);

    const IRExpression* getCondition() const noexcept;

    const IRStatement* getStatement() const noexcept;

    void setWhileSt(std::unique_ptr<IRExpression> cond, std::unique_ptr<IRStatement> st, std::unique_ptr<IRTemporary> temp = nullptr);

    const IRTemporary* getTemporaries() const noexcept;

    bool hasTemporaries() const noexcept;

    void print(size_t offset) const override;

private:
    std::unique_ptr<IRExpression> condition;
    std::unique_ptr<IRStatement> statement;
    std::unique_ptr<IRTemporary> temporaries;
};


#endif