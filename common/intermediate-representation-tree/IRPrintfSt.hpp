#ifndef IRPRINTF_ST_HPP
#define IRPRINTF_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "defs/IRTree_defs.hpp"

/** 
 * @class IRPrintfSt
 * @brief IRT representation of the printf statement node
*/
class IRPrintfSt final : public IRStatement {
public:
    IRPrintfSt(IRNodeType ntype);

    /** 
     * @brief getter for the expression of the printf statement
     * @returns const pointer to the expression of the printf statement
    */
    const IRExpression* getExp() const noexcept;

    /** 
     * @brief initializes the expression of the printf statement
     * @param _exp - pointer to the expression that is being printed
     * @param temp - pointer to the temporaries of the expression
    */
    void setExp(std::unique_ptr<IRExpression> _exp, std::unique_ptr<IRTemporary> temp = nullptr);

    /** 
     * @brief getter for the temporaries of the printf statement
     * @returns const pointer to the temporaries
    */
    const IRTemporary* getTemporaries() const noexcept;

    /** 
     * @brief initializes temporaries of the printf statement
     * @param temp - pointer to the temporaries
     * @returns void
    */
    void setTemporaries(std::unique_ptr<IRTemporary> temp);

    /** 
     * @brief checks if printf statement has temporaries
     * @returns true if printf has temporaries, false otherwise
    */
    bool hasTemporaries() const noexcept;

    /** 
     * @brief prints formatted string of the printf statement node
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