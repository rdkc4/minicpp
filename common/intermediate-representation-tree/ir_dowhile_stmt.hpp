#ifndef IR_DOWHILE_STMT_HPP
#define IR_DOWHILE_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_temporary_expr.hpp"
#include "../visitor/ir_visitor.hpp"

/**
 * @class IRDoWhileStmt
 * @brief IRT representation of the do-while statement
*/
class IRDoWhileStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt do-while statement
    */
    IRDoWhileStmt();

    /**
     * @brief getter for the condition of the do-while statement
     * @returns pointer or const pointer to the relational expression
    */
    template<typename Self>
    decltype(auto) getConditionExpr(this Self&& self) noexcept {
        return std::forward<Self>(self).conditionExpr.get();
    }

    /**
     * @brief getter for the statement node of the do-while statement
     * @returns pointer or const pointer to the statement node
    */
    template<typename Self>
    decltype(auto) getStmt(this Self&& self) noexcept {
        return std::forward<Self>(self).stmt.get();
    }

    /**
     * @brief initializes the do-while statement node
     * @param condExpr - pointer to the condition of the do-while statement node
     * @param statement - pointer to the statement of the do-while statement node
     * @param tempExpr - pointer to the temporaries of the do-while statement node, default nullptr
    */
    void setDoWhileStmt(std::unique_ptr<IRExpr> condExpr, std::unique_ptr<IRStmt> statement, std::unique_ptr<IRTemporaryExpr> tempExpr = nullptr);

    /**
     * @brief getter for the temporaries of the do-while statement node
     * @returns pointer or const pointer to the temporaries node
    */
    template<typename Self>
    decltype(auto) getTemporaryExpr(this Self&& self) noexcept {
        return std::forward<Self>(self).temporaryExpr.get();
    }

    /**
     * @brief checks if the do-while statement has any temporaries
     * @returns true if do-while statement hasbtemporaries, false otherwise
    */
    bool hasTemporaryExpr() const noexcept;

    /**
     * @brief accepts the ir visitor
     * @param visitor - reference to an ir visitor
    */
    void accept(IRVisitor& visitor) override;

private:
    /// pointer to the relational expression of the do-while statement
    std::unique_ptr<IRExpr> conditionExpr;

    /// pointer to the statement of the do-while statement
    std::unique_ptr<IRStmt> stmt;

    /// pointer to the temporaries of the do-while statement
    std::unique_ptr<IRTemporaryExpr> temporaryExpr;
};

#endif
