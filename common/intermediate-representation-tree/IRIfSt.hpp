#ifndef IRIF_ST_HPP
#define IRIF_ST_HPP

#include <vector>
#include <memory>
#include <tuple>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "defs/IRTree_defs.hpp"

class IRIfSt final : public IRStatement {
public:
    IRIfSt(IRNodeType ntype);

    const std::vector<std::unique_ptr<IRExpression>>& getConditions() const noexcept;

    const std::vector<std::unique_ptr<IRStatement>>& getStatements() const noexcept;

    const std::vector<std::unique_ptr<IRTemporary>>& getTemporaries() const noexcept;

    size_t getConditionCount() const noexcept;

    void addIf(std::unique_ptr<IRExpression> cond, std::unique_ptr<IRStatement> statement, std::unique_ptr<IRTemporary> temp = nullptr);

    void addElse(std::unique_ptr<IRStatement> statement);

    bool hasElse() const noexcept;

    const IRStatement* getElseStatement() const noexcept;

    const std::tuple<const IRExpression*, const IRStatement*, const IRTemporary*> getIfAtN(size_t n) const noexcept;

    void print(size_t offset) const override;

private:
    std::vector<std::unique_ptr<IRExpression>> conditions;
    std::vector<std::unique_ptr<IRStatement>> statements;
    std::vector<std::unique_ptr<IRTemporary>> temporaries;
};


#endif