#ifndef IRRETURN_ST_HPP
#define IRRETURN_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "defs/IRTree_defs.hpp"

/** 
 * @class IRReturnSt
 * @brief IRT representation of the return statement
*/
class IRReturnSt final : public IRStatement {
public:
    IRReturnSt(IRNodeType ntype);

    /** 
     * @brief getter for the expression of the return statement
     * @returns const pointer to the expression
    */
    const IRExpression* getExp() const noexcept;

    /** 
     * @brief initializes the expression of the return statement
     * @param _exp - pointer to the expression
     * @param temp - pointer to the temporaries of the expression, default nullptr
    */
    void setExp(std::unique_ptr<IRExpression> _exp, std::unique_ptr<IRTemporary> temp = nullptr);

    /** 
     * @brief getter for the temporaries
     * @returns const pointer to the temporary
    */
    const IRTemporary* getTemporaries() const noexcept;

    /** 
     * @brief checks if return-statement returns value
     * @returns true if return-statement returns value, false otherwise
    */
    bool returns() const noexcept;

    /** 
     * @brief checks if return statement has temporaries
     * @returns true if return statement has temporaries, false otherwise
    */
    bool hasTemporaries() const noexcept;

    /** 
     * @brief prints formatted string of the return statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::unique_ptr<IRExpression> exp;
    std::unique_ptr<IRTemporary> temporaries;
};


#endif