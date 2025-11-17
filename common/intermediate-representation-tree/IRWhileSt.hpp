#ifndef IRWHILE_ST_HPP
#define IRWHILE_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "defs/IRTree_defs.hpp"

/** 
 * @class IRWhileSt
 * @brief IRT representation of the while statement
*/
class IRWhileSt final : public IRStatement {
public:
    IRWhileSt(IRNodeType ntype);

    /** 
     * @brief getter for the condition of the while statement
     * @returns const pointer to relational expression
    */
    const IRExpression* getCondition() const noexcept;

    /** 
     * @brief getter for the statement of the while statement
     * @returns const pointer to the statement
    */
    const IRStatement* getStatement() const noexcept;

    /** 
     * @brief initializes the while statement
     * @param cond - pointer to the relational expression
     * @param st - pointer to the statement
     * @param temp - pointer to the temporary, default nullptr
     * @returns void
    */
    void setWhileSt(std::unique_ptr<IRExpression> cond, std::unique_ptr<IRStatement> st, std::unique_ptr<IRTemporary> temp = nullptr);

    /** 
     * @brief getter for the temporaries
     * @returns const pointer to the temporary
    */
    const IRTemporary* getTemporaries() const noexcept;

    /** 
     * @brief checks if the while statement has temporaries
     * @returns true if while statement has temporaries, false otherwise
    */
    bool hasTemporaries() const noexcept;

    /** 
     * @brief prints formatted string of the while statement node
     * @note debugging purposes
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