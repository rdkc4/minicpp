#ifndef IR_SWITCH_STMT_HPP
#define IR_SWITCH_STMT_HPP

#include <memory>
#include <vector>

#include "ir_stmt.hpp"
#include "ir_id_expr.hpp"
#include "ir_case_stmt.hpp"
#include "ir_default_stmt.hpp"
#include "defs/ir_defs.hpp"
#include "../visitor/ir_visitor.hpp"

/** 
 * @class IRSwitchStmt
 * @brief IRT representation of the switch statement
*/
class IRSwitchStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt switch-statement
     * @param ntype - type of the irt node
    */
    IRSwitchStmt(IRNodeType ntype);

    /** 
     * @brief getter for the variable of the switch statement
     * @returns pointer or const pointer to the id of the switch statement
    */
    template<typename Self>
    decltype(auto) getVariable(this Self&& self) noexcept {
        return std::forward<Self>(self).variable.get();
    }

    /** 
     * @brief initializes the variable of the switch statement
     * @param var - pointer to the id
    */
    void setVariable(std::unique_ptr<IRIdExpr> var);

    /** 
     * @brief getter for the cases of the switch statement
     * @returns reference to a const vector of pointers to cases
    */
    const std::vector<std::unique_ptr<IRCaseStmt>>& getCases() const noexcept;

    /** 
     * @brief getter for the case at the specified position
     * @param n - position of the case
     * @returns const pointer to the case
    */
    const IRCaseStmt* getCaseAtN(size_t n) const noexcept;

    /** 
     * @brief adds new case to the switch statement
     * @param _case - pointer to the case
    */
    void addCase(std::unique_ptr<IRCaseStmt> _case);

    /** 
     * @brief getter for the default case of the switch statement
     * @returns pointer or const pointer to the default case
    */
    template<typename Self>
    decltype(auto) getDefault(this Self&& self) noexcept {
        return std::forward<Self>(self)._default.get();
    }

    /** 
     * @brief initializes the default case of the switch statement
     * @param _swDefault - pointer to the default case
    */
    void setDefault(std::unique_ptr<IRDefaultStmt> _swDefault);

    /** 
     * @brief checks if the switch statement has default case
     * @returns true if switch statement has default, false otherwise
    */
    bool hasDefault() const noexcept;

    /** 
     * @brief getter for the count of cases
     * @returns number of cases of the switch statement
     * @note default case is not included
    */
    size_t getCaseCount() const noexcept;

    void accept(IRVisitor& visitor) override;

private:
    /// pointer to the id of the variable of the switch-statement
    std::unique_ptr<IRIdExpr> variable;

    /// vector of pointers to cases of the switch-statement
    std::vector<std::unique_ptr<IRCaseStmt>> cases;

    /// pointer to default case of the switch-statement
    std::unique_ptr<IRDefaultStmt> _default;
    
};


#endif