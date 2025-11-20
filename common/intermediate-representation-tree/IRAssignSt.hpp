#ifndef IRASSIGN_ST_HPP
#define IRASSIGN_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "IRId.hpp"
#include "defs/IRTree_defs.hpp"

/**
 * @class IRAssignSt
 * @brief IRT representation of the assignment statement
*/
class IRAssignSt final : public IRStatement {
public:
    /** 
     * @brief Creates the instance of the irt assignment statement
     * @param ntype - type of the irt node
    */
    IRAssignSt(IRNodeType ntype);

    /**
     * @brief getter for the id of the variable of the assignment statement
     * @returns const pointer to the id node
    */
    const IRId* getVariable() const noexcept;

    /**
     * @brief getter for the expression that is being assigned to variable
     * @warning nullable
     * @returns const pointer to the expression node
    */
    const IRExpression* getExp() const noexcept;

    /**
     * @brief getter for the temporary variables of the assignment statement
     * @warning nullable
     * @returns const pointer to the temporary node
    */
    const IRTemporary* getTemporaries() const noexcept;

    /**
     * @brief initializes assignment statement
     * @param var - pointer to the id of the variable
     * @param _exp - pointer to the expression
     * @param temp - pointer to the temporary (default nullptr)
     * @returns void
    */
    void setAssignSt(std::unique_ptr<IRId> var, std::unique_ptr<IRExpression> _exp, std::unique_ptr<IRTemporary> temp = nullptr);

    /**
     * @brief checks if assignment statement has temporaries
     * @returns true if there are temporaries, false otherwise
    */
    bool hasTemporaries() const noexcept;

    /**
     * @brief prints formatted string of the assignment statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// pointer to the id of the variable
    std::unique_ptr<IRId> variable;
    /// pointer to the expression assigned to the variable
    std::unique_ptr<IRExpression> exp;
    /// pointer to the temporaries of the assignment statement
    std::unique_ptr<IRTemporary> temporaries;
};


#endif
