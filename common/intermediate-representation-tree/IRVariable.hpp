#ifndef IRVARIABLE_HPP
#define IRVARIABLE_HPP

#include <memory>
#include <string>

#include "IRStatement.hpp"
#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

/** 
 * @class IRVariable
 * @brief IRT representation of the variable
*/
class IRVariable final : public IRStatement {
public:
    IRVariable(IRNodeType ntype, std::string_view varName, Types type);

    /** 
     * @brief getter for the value assigned to the variable
     * @returns const pointer to the expression
    */
    const IRExpression* getAssign() const noexcept;

    /** 
     * @brief assigns the value to the variable
     * @param assign - pointer to the expression
     * @param temp - pointer to the temporaries, default nullptr
     * @returns void
    */
    void setAssign(std::unique_ptr<IRExpression> assign, std::unique_ptr<IRTemporary> temp = nullptr);

    /** 
     * @brief getter for the name of the variable
     * @returns reference to the name of the variable as const string
    */
    const std::string& getVarName() const noexcept;

    /** 
     * @brief initializes the name of the variable
     * @param var - reference to the name of the variable
     * @returns void
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
     * @returns void
    */
    void setValue(const std::string& val);

    /** 
     * @brief getter for the type of the variable
     * @returns type of the variable
    */
    Types getType() const noexcept;

    /** 
     * @brief initializes the type of the variable
     * @param t - type of the variable
     * @returns void
    */
    void setType(Types t) noexcept;

    /** 
     * @brief checks if variable is assigned value
     * @returns true if variable has an assignment, false otherwise
    */
    bool hasAssign() const noexcept;

    /** 
     * @brief getter for the temporaries of the variable
     * @returns const pointer to the temporary
    */
    const IRTemporary* getTemporaries() const noexcept;

    /** 
     * @brief checks if variable has temporaries
     * @returns true if variable has temporaries, false otherwise
    */
    bool hasTemporaries() const noexcept;

    /** 
     * @brief prints formatted string of the variable node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::string varName;
    std::string value;
    Types type;
    std::unique_ptr<IRExpression> assignment;
    std::unique_ptr<IRTemporary> temporaries;
};


#endif