#ifndef IRIF_ST_HPP
#define IRIF_ST_HPP

#include <vector>
#include <memory>
#include <tuple>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "defs/IRTree_defs.hpp"

/** 
 * @class IRIfSt
 * @brief IRT representation of the if-statement
*/
class IRIfSt final : public IRStatement {
public:
    IRIfSt(IRNodeType ntype);

    /** 
     * @brief getter for the conditions of the if-statement
     * @returns reference to a const vector of pointers to the relational expressions
    */
    const std::vector<std::unique_ptr<IRExpression>>& getConditions() const noexcept;

    /** 
     * @brief getter for the statements of the if-statement
     * @returns reference to a const vector of pointers to the statements
    */
    const std::vector<std::unique_ptr<IRStatement>>& getStatements() const noexcept;

    /** 
     * @brief getter for the temporaries of the if-statement
     * @returns reference to a const vector of pointers to the temporaries
    */
    const std::vector<std::unique_ptr<IRTemporary>>& getTemporaries() const noexcept;

    /** 
     * @brief getter for the number of if/else-if statements
     * @returns number of conditions
    */
    size_t getConditionCount() const noexcept;

    /** 
     * @brief initializes if/else-if statement
     * @param cond - pointer to a relational expression of the if/else-if statement
     * @param statement - pointer to a statement of the if/else-if statement
     * @param temp - pointer to a temporary of the if/else-if statement, default nullptr
     * @returns void
    */
    void addIf(std::unique_ptr<IRExpression> cond, std::unique_ptr<IRStatement> statement, std::unique_ptr<IRTemporary> temp = nullptr);

    /** 
     * @brief initializes else-statement
     * @param statement - pointer to a statement of the else-statement
     * @returns void
    */
    void addElse(std::unique_ptr<IRStatement> statement);

    /** 
     * @brief checks if if-statement ends with else-statement
     * @returns true if if-statements ends with else, false otherwise
    */
    bool hasElse() const noexcept;

    /** 
     * @brief getter for the statement of the else-statement
     * @returns const reference to a statement of the else-statement
    */
    const IRStatement* getElseStatement() const noexcept;

    /** 
     * @brief getter for the if-statement at specified position
     * @param n - position of the if/else-if statement
     * @returns const tuple of pointers to condition, statement and temporary
    */
    const std::tuple<const IRExpression*, const IRStatement*, const IRTemporary*> getIfAtN(size_t n) const noexcept;

    /** 
     * @brief prints formatted string of the if-statement node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::vector<std::unique_ptr<IRExpression>> conditions;
    std::vector<std::unique_ptr<IRStatement>> statements;
    std::vector<std::unique_ptr<IRTemporary>> temporaries;
};


#endif