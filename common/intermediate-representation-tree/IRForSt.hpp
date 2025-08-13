#ifndef IRFOR_ST_HPP
#define IRFOR_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRAssignSt.hpp"
#include "defs/IRTree_defs.hpp"

class IRForSt final : public IRStatement {
public:
    IRForSt(IRNodeType ntype);

    const IRAssignSt* getInitializer() const noexcept;

    const IRExpression* getCondition() const noexcept;

    const IRAssignSt* getIncrementer() const noexcept;

    const IRStatement* getStatement() const noexcept;

    void setForSt(std::unique_ptr<IRAssignSt> init, std::unique_ptr<IRExpression> cond, std::unique_ptr<IRAssignSt> inc, std::unique_ptr<IRStatement> st, std::unique_ptr<IRTemporary> temp = nullptr);

    bool hasInitializer() const noexcept;

    bool hasCondition() const noexcept;

    bool hasIncrementer() const noexcept;

    const IRTemporary* getTemporaries() const noexcept;

    bool hasTemporaries() const noexcept;

    void print(size_t offset) const override;

private:
    std::unique_ptr<IRAssignSt> initializer;
    std::unique_ptr<IRExpression> condition;
    std::unique_ptr<IRAssignSt> incrementer;
    std::unique_ptr<IRStatement> statement;
    std::unique_ptr<IRTemporary> temporaries;
};

#endif