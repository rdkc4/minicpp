#ifndef IR_DOWHILE_STMT_HPP
#define IR_DOWHILE_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_temporary_expr.hpp"
#include "defs/ir_defs.hpp"

/**
 * @class IRDoWhileStmt
 * @brief IRT representation of the do-while statement
*/
class IRDoWhileStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt do-while statement
     * @param ntype - type of the irt node
    */
    IRDoWhileStmt(IRNodeType ntype);

    /**
     * @brief getter for the condition of the do-while statement
     * @returns const pointer to the relational expression
    */
    const IRExpr* getCondition() const noexcept;

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
    */
    void setDoWhileSt(std::unique_ptr<IRExpr> cond, std::unique_ptr<IRStmt> st, std::unique_ptr<IRTemporaryExpr> temp = nullptr);

    /**
     * @brief getter for the temporaries of the do-while statement node
     * @returns const pointer to the temporaries node
    */
    const IRTemporaryExpr* getTemporaries() const noexcept;

    /**
     * @brief checks if the do-while statement has any temporaries
     * @returns true if do-while statement hasbtemporaries, false otherwise
    */
    bool hasTemporaries() const noexcept;

private:
    /// pointer to the relational expression of the do-while statement
    std::unique_ptr<IRExpr> condition;

    /// pointer to the statement of the do-while statement
    std::unique_ptr<IRStmt> statement;

    /// pointer to the temporaries of the do-while statement
    std::unique_ptr<IRTemporaryExpr> temporaries;
};

#endif
