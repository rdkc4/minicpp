#ifndef IR_VARIABLE_DECL_STMT_HPP
#define IR_VARIABLE_DECL_STMT_HPP

#include <memory>
#include <string>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_temporary_expr.hpp"
#include "defs/ir_defs.hpp"
#include "../defs/defs.hpp"

/** 
 * @class IRVariableDeclStmt
 * @brief IRT representation of the variable
*/
class IRVariableDeclStmt final : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt variable declaration
     * @param ntype - type of the irt node
     * @param varName - name of the variable
     * @param type - type of the variable
    */
    IRVariableDeclStmt(IRNodeType ntype, std::string_view varName, Type type);

    /** 
     * @brief getter for the value assigned to the variable
     * @returns const pointer to the expression
    */
    const IRExpr* getAssign() const noexcept;

    /** 
     * @brief assigns the value to the variable
     * @param assign - pointer to the expression
     * @param temp - pointer to the temporaries, default nullptr
    */
    void setAssign(std::unique_ptr<IRExpr> assign, std::unique_ptr<IRTemporaryExpr> temp = nullptr);

    /** 
     * @brief getter for the name of the variable
     * @returns reference to the name of the variable as const string
    */
    const std::string& getVarName() const noexcept;

    /** 
     * @brief initializes the name of the variable
     * @param var - reference to the name of the variable
    */
    void setVarName(const std::string& var);

    /** 
     * @brief getter for the value of the variable
     * @returns reference to the value of the variable as const string
    */
    const std::string& getValue() const noexcept;

    /** 
     * @brief initializes the value of the variable
     * @param val - reference to a value of the variable as const string
    */
    void setValue(const std::string& val);

    /** 
     * @brief getter for the type of the variable
     * @returns type of the variable
    */
    Type getType() const noexcept;

    /** 
     * @brief initializes the type of the variable
     * @param t - type of the variable
    */
    void setType(Type t) noexcept;

    /** 
     * @brief checks if variable is assigned value
     * @returns true if variable has an assignment, false otherwise
    */
    bool hasAssign() const noexcept;

    /** 
     * @brief getter for the temporaries of the variable
     * @returns const pointer to the temporary
    */
    const IRTemporaryExpr* getTemporaries() const noexcept;

    /** 
     * @brief checks if variable has temporaries
     * @returns true if variable has temporaries, false otherwise
    */
    bool hasTemporaries() const noexcept;

private:
    /// name of the variable
    std::string varName;

    /// value of the variable
    std::string value;

    /// type of the variable
    Type type;

    /// pointer to the expression assigned to the variable
    std::unique_ptr<IRExpr> assignment;

    /// pointer to the temporaries of the variable
    std::unique_ptr<IRTemporaryExpr> temporaries;
    
};


#endif