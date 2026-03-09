#ifndef IR_ASSIGN_STMT_HPP
#define IR_ASSIGN_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_temporary_expr.hpp"
#include "ir_id_expr.hpp"
#include "defs/ir_defs.hpp"
#include "../visitor/ir_visitor.hpp"

/**
 * @class IRAssignStmt
 * @brief IRT representation of the assignment statement
*/
class IRAssignStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt assignment statement
     * @param ntype - type of the irt node
    */
    IRAssignStmt(IRNodeType ntype);

    /**
     * @brief getter for the id of the variable of the assignment statement
     * @returns pointer or const pointer to the id node
    */
    template<typename Self>
    decltype(auto) getVariable(this Self&& self) noexcept {
        return std::forward<Self>(self).variable.get();
    }

    /**
     * @brief getter for the expression that is being assigned to variable
     * @returns pointer or const pointer to the expression node
    */
    template<typename Self>
    decltype(auto) getExp(this Self&& self) noexcept {
        return std::forward<Self>(self).exp.get();
    }

    /**
     * @brief getter for the temporary variables of the assignment statement
     * @warning nullable
     * @returns const pointer to the temporary node
    */
    template<typename Self>
    decltype(auto) getTemporaries(this Self&& self) noexcept {
        return std::forward<Self>(self).temporaries.get();
    }

    /**
     * @brief initializes assignment statement
     * @param var - pointer to the id of the variable
     * @param _exp - pointer to the expression
     * @param temp - pointer to the temporary (default nullptr)
    */
    void setAssignSt(std::unique_ptr<IRIdExpr> var, std::unique_ptr<IRExpr> _exp, std::unique_ptr<IRTemporaryExpr> temp = nullptr);

    /**
     * @brief checks if assignment statement has temporaries
     * @returns true if there are temporaries, false otherwise
    */
    bool hasTemporaries() const noexcept;

    void accept(IRVisitor& visitor) override;

private:
    /// pointer to the id of the variable
    std::unique_ptr<IRIdExpr> variable;

    /// pointer to the expression assigned to the variable
    std::unique_ptr<IRExpr> exp;

    /// pointer to the temporaries of the assignment statement
    std::unique_ptr<IRTemporaryExpr> temporaries;
};


#endif
