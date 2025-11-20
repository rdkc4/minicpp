#ifndef IRFOR_ST_HPP
#define IRFOR_ST_HPP

#include <memory>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRAssignSt.hpp"
#include "defs/IRTree_defs.hpp"

/**
 * @class IRForSt
 * @brief IRT representation for the for-statement
*/
class IRForSt final : public IRStatement {
public:
    /** 
     * @brief Creates the instance of the irt for-statement
     * @param ntype - type of the irt node
    */
    IRForSt(IRNodeType ntype);

    /**
     * @brief getter for the initializer of the for-statement node
     * @warning nullable
     * @returns const pointer to the assignment statement
    */
    const IRAssignSt* getInitializer() const noexcept;

    /**
     * @brief getter for the condition of the for-statement node
     * @warning nullable
     * @returns const pointer to the relational expression
    */
    const IRExpression* getCondition() const noexcept;

    /**
     * @brief getter for the incrementer of the for-statement node
     * @warning nullable
     * @returns const pointer to the assignment statement
    */
    const IRAssignSt* getIncrementer() const noexcept;

    /**
     * @brief getter for the statement of the for-statement node
     * @returns pointer to the statement
    */
    const IRStatement* getStatement() const noexcept;

    /**
     * @brief initializes the for-statement node
     * @param init - pointer to the assignment statement
     * @param cond - pointer to the relational expression
     * @param inc - pointer to the assignment statement
     * @param st - pointer to the statement
     * @param temp - pointer to the temporaries, default nullptr
     * @returns void
    */
    void setForSt(std::unique_ptr<IRAssignSt> init, std::unique_ptr<IRExpression> cond, std::unique_ptr<IRAssignSt> inc, std::unique_ptr<IRStatement> st, std::unique_ptr<IRTemporary> temp = nullptr);

    /**
     * @brief checks if for-statement has initializes
     * @returns true when initializes is not nullptr, false otherwise
    */
    bool hasInitializer() const noexcept;

    /**
     * @brief checks if for-statement has condition
     * @returns true if condition is not null, false otherwise
    */
    bool hasCondition() const noexcept;

    /**
     * @brief checks if for-statement has incrementer
     * @returns true if the incrementer is not nullptr, false otherwise
    */
    bool hasIncrementer() const noexcept;

    /**
     * @brief getter for the temporaries of the for-statement
     * @returns const pointer to the temporaries
    */
    const IRTemporary* getTemporaries() const noexcept;

    /**
     * @brief checks if for-statement has temporaries
     * @returns true if temporaries are not nullptr, false otherwise
    */
    bool hasTemporaries() const noexcept;

    /**
     * @brief prints formatted string of the for-statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// pointer to the assignment statement of the initializer
    std::unique_ptr<IRAssignSt> initializer;
    /// pointer to the relational expression of the for-statement
    std::unique_ptr<IRExpression> condition;
    /// pointer to the assignment statement of the incrementer
    std::unique_ptr<IRAssignSt> incrementer;
    /// pointer to the statement of the for-statement
    std::unique_ptr<IRStatement> statement;
    /// pointer to temporaries of the for-statement
    std::unique_ptr<IRTemporary> temporaries;
};

#endif
