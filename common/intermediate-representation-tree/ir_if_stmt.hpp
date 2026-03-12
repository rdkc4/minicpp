#ifndef IR_IF_STMT_HPP
#define IR_IF_STMT_HPP

#include <vector>
#include <memory>
#include <tuple>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_temporary_expr.hpp"
#include "../visitor/ir_visitor.hpp"

/** 
 * @class IRIfStmt
 * @brief IRT representation of the if-statement
*/
class IRIfStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt if-statement
    */
    IRIfStmt();

    /** 
     * @brief getter for the conditions of the if-statement
     * @returns reference to a const vector of pointers to the relational expressions
    */
    const std::vector<std::unique_ptr<IRExpr>>& getConditionExprs() const noexcept;

    /** 
     * @brief getter for the statements of the if-statement
     * @returns reference to a const vector of pointers to the statements
    */
    const std::vector<std::unique_ptr<IRStmt>>& getStmts() const noexcept;

    /** 
     * @brief getter for the temporaries of the if-statement
     * @returns reference to a const vector of pointers to the temporaries
    */
    const std::vector<std::unique_ptr<IRTemporaryExpr>>& getTemporaryExprs() const noexcept;

    /** 
     * @brief getter for the number of if/else-if statements
     * @returns number of conditions
    */
    size_t getConditionCount() const noexcept;

    /** 
     * @brief initializes if/else-if statement
     * @param condExpr - pointer to a relational expression of the if/else-if statement
     * @param statement - pointer to a statement of the if/else-if statement
     * @param tempExpr - pointer to a temporary of the if/else-if statement, default nullptr
    */
    void addIfStmt(std::unique_ptr<IRExpr> condExpr, std::unique_ptr<IRStmt> statement, std::unique_ptr<IRTemporaryExpr> tempExpr = nullptr);

    /** 
     * @brief initializes else-statement
     * @param statement - pointer to a statement of the else-statement
    */
    void addElseStmt(std::unique_ptr<IRStmt> statement);

    /** 
     * @brief checks if if-statement ends with else-statement
     * @returns true if if-statements ends with else, false otherwise
    */
    bool hasElseStmt() const noexcept;

    /** 
     * @brief getter for the statement of the else-statement
     * @returns const pointer to a statement of the else-statement
    */
    const IRStmt* getElseStmt() const noexcept;

    /** 
     * @brief getter for the if-statement at specified position
     * @param n - position of the if/else-if statement
     * @returns const tuple of pointers to condition, statement and temporary
    */
    const std::tuple<const IRExpr*, const IRStmt*, const IRTemporaryExpr*> getIfStmtAtN(size_t n) const noexcept;

    /**
     * @brief accepts the ir visitor
     * @param visitor - reference to an ir visitor
    */
    void accept(IRVisitor& visitor) override;

private:
    /// vector of pointers to relational expressions of the if-statement
    std::vector<std::unique_ptr<IRExpr>> conditionExprs;

    /// vector of pointers to statements of the if-statement
    std::vector<std::unique_ptr<IRStmt>> stmts;

    /// vector of pointers to temporaries of the if-statement
    std::vector<std::unique_ptr<IRTemporaryExpr>> temporaryExprs;
    
};


#endif