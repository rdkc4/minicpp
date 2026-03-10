#ifndef IR_FOR_STMT_HPP
#define IR_FOR_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_assign_stmt.hpp"
#include "defs/ir_defs.hpp"
#include "../visitor/ir_visitor.hpp"

/**
 * @class IRForStmt
 * @brief IRT representation for the for-statement
*/
class IRForStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt for-statement
     * @param ntype - type of the irt node
    */
    IRForStmt(IRNodeType ntype);

    /**
     * @brief getter for the initializer of the for-statement node
     * @warning nullable
     * @returns pointer const pointer to the assignment statement
    */
    template<typename Self>
    decltype(auto) getInitializer(this Self&& self) noexcept {
        return std::forward<Self>(self).initializer.get();
    }

    /**
     * @brief getter for the condition of the for-statement node
     * @warning nullable
     * @returns pointer const pointer to the relational expression
    */
    template<typename Self>
    decltype(auto) getCondition(this Self&& self) noexcept {
        return std::forward<Self>(self).condition.get();
    }

    /**
     * @brief getter for the incrementer of the for-statement node
     * @warning nullable
     * @returns pointer or const pointer to the assignment statement
    */
    template<typename Self>
    decltype(auto) getIncrementer(this Self&& self) noexcept {
        return std::forward<Self>(self).incrementer.get();
    }

    /**
     * @brief getter for the statement of the for-statement node
     * @returns pointer or const pointer to the statement
    */
    template<typename Self>
    decltype(auto) getStatement(this Self&& self) noexcept {
        return std::forward<Self>(self).statement.get();
    }

    /**
     * @brief initializes the for-statement node
     * @param init - pointer to the assignment statement
     * @param cond - pointer to the relational expression
     * @param inc - pointer to the assignment statement
     * @param st - pointer to the statement
     * @param temp - pointer to the temporaries, default nullptr
    */
    void setForSt(std::unique_ptr<IRAssignStmt> init, std::unique_ptr<IRExpr> cond, std::unique_ptr<IRAssignStmt> inc, std::unique_ptr<IRStmt> st, std::unique_ptr<IRTemporaryExpr> temp = nullptr);

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
     * @returns pointer or const pointer to the temporaries
    */
    template<typename Self>
    decltype(auto) getTemporaries(this Self&& self) noexcept {
        return std::forward<Self>(self).temporaries.get();
    }

    /**
     * @brief checks if for-statement has temporaries
     * @returns true if temporaries are not nullptr, false otherwise
    */
    bool hasTemporaries() const noexcept;

    /**
     * @brief accepts the ir visitor
     * @param visitor - reference to an ir visitor
    */
    void accept(IRVisitor& visitor) override;

private:
    /// pointer to the assignment statement of the initializer
    std::unique_ptr<IRAssignStmt> initializer;

    /// pointer to the relational expression of the for-statement
    std::unique_ptr<IRExpr> condition;

    /// pointer to the assignment statement of the incrementer
    std::unique_ptr<IRAssignStmt> incrementer;

    /// pointer to the statement of the for-statement
    std::unique_ptr<IRStmt> statement;
    
    /// pointer to temporaries of the for-statement
    std::unique_ptr<IRTemporaryExpr> temporaries;
};

#endif
