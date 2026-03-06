#ifndef IR_FOR_STMT_HPP
#define IR_FOR_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_assign_stmt.hpp"
#include "defs/IRTree_defs.hpp"

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
     * @returns const pointer to the assignment statement
    */
    const IRAssignStmt* getInitializer() const noexcept;

    /**
     * @brief getter for the condition of the for-statement node
     * @warning nullable
     * @returns const pointer to the relational expression
    */
    const IRExpr* getCondition() const noexcept;

    /**
     * @brief getter for the incrementer of the for-statement node
     * @warning nullable
     * @returns const pointer to the assignment statement
    */
    const IRAssignStmt* getIncrementer() const noexcept;

    /**
     * @brief getter for the statement of the for-statement node
     * @returns pointer to the statement
    */
    const IRStmt* getStatement() const noexcept;

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
     * @returns const pointer to the temporaries
    */
    const IRTemporaryExpr* getTemporaries() const noexcept;

    /**
     * @brief checks if for-statement has temporaries
     * @returns true if temporaries are not nullptr, false otherwise
    */
    bool hasTemporaries() const noexcept;

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
