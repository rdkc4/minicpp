#ifndef IR_IF_STMT_HPP
#define IR_IF_STMT_HPP

#include <vector>
#include <memory>
#include <tuple>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_temporary_expr.hpp"
#include "defs/ir_defs.hpp"

/** 
 * @class IRIfStmt
 * @brief IRT representation of the if-statement
*/
class IRIfStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt if-statement
     * @param ntype - type of the irt node
    */
    IRIfStmt(IRNodeType ntype);

    /** 
     * @brief getter for the conditions of the if-statement
     * @returns reference to a const vector of pointers to the relational expressions
    */
    const std::vector<std::unique_ptr<IRExpr>>& getConditions() const noexcept;

    /** 
     * @brief getter for the statements of the if-statement
     * @returns reference to a const vector of pointers to the statements
    */
    const std::vector<std::unique_ptr<IRStmt>>& getStatements() const noexcept;

    /** 
     * @brief getter for the temporaries of the if-statement
     * @returns reference to a const vector of pointers to the temporaries
    */
    const std::vector<std::unique_ptr<IRTemporaryExpr>>& getTemporaries() const noexcept;

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
    */
    void addIf(std::unique_ptr<IRExpr> cond, std::unique_ptr<IRStmt> statement, std::unique_ptr<IRTemporaryExpr> temp = nullptr);

    /** 
     * @brief initializes else-statement
     * @param statement - pointer to a statement of the else-statement
    */
    void addElse(std::unique_ptr<IRStmt> statement);

    /** 
     * @brief checks if if-statement ends with else-statement
     * @returns true if if-statements ends with else, false otherwise
    */
    bool hasElse() const noexcept;

    /** 
     * @brief getter for the statement of the else-statement
     * @returns const reference to a statement of the else-statement
    */
    const IRStmt* getElseStatement() const noexcept;

    /** 
     * @brief getter for the if-statement at specified position
     * @param n - position of the if/else-if statement
     * @returns const tuple of pointers to condition, statement and temporary
    */
    const std::tuple<const IRExpr*, const IRStmt*, const IRTemporaryExpr*> getIfAtN(size_t n) const noexcept;

private:
    /// vector of pointers to relational expressions of the if-statement
    std::vector<std::unique_ptr<IRExpr>> conditions;

    /// vector of pointers to statements of the if-statement
    std::vector<std::unique_ptr<IRStmt>> statements;

    /// vector of pointers to temporaries of the if-statement
    std::vector<std::unique_ptr<IRTemporaryExpr>> temporaries;
    
};


#endif