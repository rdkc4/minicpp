#ifndef IR_WHILE_STMT_HPP
#define IR_WHILE_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_temporary_expr.hpp"
#include "defs/ir_defs.hpp"
#include "../visitor/ir_visitor.hpp"

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
     * @returns pointer or const pointer to relational expression
    */
    template<typename Self>
    decltype(auto) getCondition(this Self&& self) noexcept {
        return std::forward<Self>(self).condition.get();
    }

    /** 
     * @brief getter for the statement of the while statement
     * @returns pointer or const pointer to the statement
    */
    template<typename Self>
    decltype(auto) getStatement(this Self&& self) noexcept {
        return std::forward<Self>(self).statement.get();
    }

    /** 
     * @brief initializes the while statement
     * @param cond - pointer to the relational expression
     * @param st - pointer to the statement
     * @param temp - pointer to the temporary, default nullptr
    */
    void setWhileSt(std::unique_ptr<IRExpr> cond, std::unique_ptr<IRStmt> st, std::unique_ptr<IRTemporaryExpr> temp = nullptr);

    /** 
     * @brief getter for the temporaries
     * @returns pointer or const pointer to the temporary
    */
    template<typename Self>
    decltype(auto) getTemporaries(this Self&& self) noexcept {
        return std::forward<Self>(self).temporaries.get();
    }

    /** 
     * @brief checks if the while statement has temporaries
     * @returns true if while statement has temporaries, false otherwise
    */
    bool hasTemporaries() const noexcept;

    void accept(IRVisitor& visitor) override;

private:
    /// pointer to the relational expression of the while-statement
    std::unique_ptr<IRExpr> condition;

    /// pointer to the statement of the while-statement
    std::unique_ptr<IRStmt> statement;

    /// pointer to the temporaries of the while-statement
    std::unique_ptr<IRTemporaryExpr> temporaries;
    
};


#endif