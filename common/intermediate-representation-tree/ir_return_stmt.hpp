#ifndef IR_RETURN_STMT_HPP
#define IR_RETURN_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_temporary_expr.hpp"
#include "defs/ir_defs.hpp"

/** 
 * @class IRReturnStmt
 * @brief IRT representation of the return statement
*/
class IRReturnStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt return statement
     * @param ntype - type of the irt node
    */
    IRReturnStmt(IRNodeType ntype);

    /** 
     * @brief getter for the expression of the return statement
     * @returns const pointer to the expression
    */
    const IRExpr* getExp() const noexcept;

    /** 
     * @brief initializes the expression of the return statement
     * @param _exp - pointer to the expression
     * @param temp - pointer to the temporaries of the expression, default nullptr
    */
    void setExp(std::unique_ptr<IRExpr> _exp, std::unique_ptr<IRTemporaryExpr> temp = nullptr);

    /** 
     * @brief getter for the temporaries
     * @returns const pointer to the temporary
    */
    const IRTemporaryExpr* getTemporaries() const noexcept;

    /** 
     * @brief checks if return-statement returns value
     * @returns true if return-statement returns value, false otherwise
    */
    bool returns() const noexcept;

    /** 
     * @brief checks if return statement has temporaries
     * @returns true if return statement has temporaries, false otherwise
    */
    bool hasTemporaries() const noexcept;

private:
    /// pointer to the expression of the return statement
    std::unique_ptr<IRExpr> exp;

    /// pointer to the temporaries of the return statement
    std::unique_ptr<IRTemporaryExpr> temporaries;
    
};


#endif