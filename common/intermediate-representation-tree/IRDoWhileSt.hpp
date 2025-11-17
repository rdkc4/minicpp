#ifndef IRDOWHILE_ST_HPP
#define IRDOWHILE_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "defs/IRTree_defs.hpp"

/**
 * @class IRDoWhileSt
 * @brief IRT representation of the do-while statement
*/
class IRDoWhileSt final : public IRStatement {
public:
    IRDoWhileSt(IRNodeType ntype);

    /**
     * @brief getter for the condition of the do-while statement
     * @returns const pointer to the relational expression
    */
    const IRExpression* getCondition() const noexcept;

    /**
     * @brief getter for the statement node of the do-while statement
     * @returns pointer or const pointer to the statement node
    */
    template<typename Self>
    decltype(auto) getStatement(this Self&& self) noexcept {
        return std::forward<Self>(self).statement.get();
    }

    /**
     * @brief initializes the do-while statement node
     * @param cond - pointer to the condition of the do-while statement node
     * @param st - pointer to the statement of the do-while statement node
     * @param temp - pointer to the temporaries of the do-while statement node, default nullptr
     * @returns void
    */
    void setDoWhileSt(std::unique_ptr<IRExpression> cond, std::unique_ptr<IRStatement> st, std::unique_ptr<IRTemporary> temp = nullptr);

    /**
     * @brief getter for the temporaries of the do-while statement node
     * @returns const pointer to the temporaries node
    */
    const IRTemporary* getTemporaries() const noexcept;

    /**
     * @brief checks if the do-while statement has any temporaries
     * @returns true if do-while statement hasbtemporaries, false otherwise
    */
    bool hasTemporaries() const noexcept;

    /**
     * @brief prints formatted string of the do-while statement node
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::unique_ptr<IRExpression> condition;
    std::unique_ptr<IRStatement> statement;
    std::unique_ptr<IRTemporary> temporaries;
};

#endif
