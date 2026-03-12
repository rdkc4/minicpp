#ifndef IR_ASSIGN_STMT_HPP
#define IR_ASSIGN_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_temporary_expr.hpp"
#include "ir_id_expr.hpp"
#include "../visitor/ir_visitor.hpp"

/**
 * @class IRAssignStmt
 * @brief IRT representation of the assignment statement
*/
class IRAssignStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt assignment statement
    */
    IRAssignStmt();

    /**
     * @brief getter for the id of the variable of the assignment statement
     * @returns pointer or const pointer to the id node
    */
    template<typename Self>
    decltype(auto) getVariableIdExpr(this Self&& self) noexcept {
        return std::forward<Self>(self).variableIdExpr.get();
    }

    /**
     * @brief getter for the expression that is being assigned to variable
     * @returns pointer or const pointer to the expression node
    */
    template<typename Self>
    decltype(auto) getAssignedExpr(this Self&& self) noexcept {
        return std::forward<Self>(self).assignedExpr.get();
    }

    /**
     * @brief getter for the temporary variables of the assignment statement
     * @warning nullable
     * @returns const pointer to the temporary node
    */
    template<typename Self>
    decltype(auto) getTemporaryExpr(this Self&& self) noexcept {
        return std::forward<Self>(self).temporaryExpr.get();
    }

    /**
     * @brief initializes assignment statement
     * @param idExpr - pointer to the id of the variable
     * @param expr - pointer to the expression assigned to the variable
     * @param tempExpr - pointer to the temporary (default nullptr)
    */
    void setAssignStmt(std::unique_ptr<IRIdExpr> idExpr, std::unique_ptr<IRExpr> expr, std::unique_ptr<IRTemporaryExpr> tempExpr = nullptr);

    /**
     * @brief checks if assignment statement has temporaries
     * @returns true if there are temporaries, false otherwise
    */
    bool hasTemporaryExpr() const noexcept;

    /**
     * @brief accepts the ir visitor
     * @param visitor - reference to an ir visitor
    */
    void accept(IRVisitor& visitor) override;

private:
    /// pointer to the id of the variable
    std::unique_ptr<IRIdExpr> variableIdExpr;

    /// pointer to the expression assigned to the variable
    std::unique_ptr<IRExpr> assignedExpr;

    /// pointer to the temporaries of the assignment statement
    std::unique_ptr<IRTemporaryExpr> temporaryExpr;
};


#endif
