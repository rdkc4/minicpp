#ifndef IR_WHILE_STMT_HPP
#define IR_WHILE_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_temporary_expr.hpp"
#include "defs/ir_defs.hpp"

/** 
 * @class IRWhileStmt
 * @brief IRT representation of the while statement
*/
class IRWhileStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt while-statement
     * @param ntype - type of the irt node
    */
    IRWhileStmt(IRNodeType ntype);

    /** 
     * @brief getter for the condition of the while statement
     * @returns const pointer to relational expression
    */
    const IRExpr* getCondition() const noexcept;

    /** 
     * @brief getter for the statement of the while statement
     * @returns const pointer to the statement
    */
    const IRStmt* getStatement() const noexcept;

    /** 
     * @brief initializes the while statement
     * @param cond - pointer to the relational expression
     * @param st - pointer to the statement
     * @param temp - pointer to the temporary, default nullptr
    */
    void setWhileSt(std::unique_ptr<IRExpr> cond, std::unique_ptr<IRStmt> st, std::unique_ptr<IRTemporaryExpr> temp = nullptr);

    /** 
     * @brief getter for the temporaries
     * @returns const pointer to the temporary
    */
    const IRTemporaryExpr* getTemporaries() const noexcept;

    /** 
     * @brief checks if the while statement has temporaries
     * @returns true if while statement has temporaries, false otherwise
    */
    bool hasTemporaries() const noexcept;

private:
    /// pointer to the relational expression of the while-statement
    std::unique_ptr<IRExpr> condition;

    /// pointer to the statement of the while-statement
    std::unique_ptr<IRStmt> statement;

    /// pointer to the temporaries of the while-statement
    std::unique_ptr<IRTemporaryExpr> temporaries;
    
};


#endif