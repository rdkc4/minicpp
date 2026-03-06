#ifndef IR_ID_EXPR_HPP
#define IR_ID_EXPR_HPP

#include <string>
#include <string_view>

#include "ir_expr.hpp"
#include "defs/ir_defs.hpp"
#include "../defs/defs.hpp"

/**
 * @class IRIdExpr
 * @brief IRT representation for the id 
*/
class IRIdExpr final : public IRExpr {
public:
    /** 
     * @brief Creates the instance of the irt id
     * @param ntype - type of the irt node
     * @param idName - name of the id
     * @param type - type of the id
    */
    IRIdExpr(IRNodeType ntype, std::string_view idName, Type type);

    /** 
     * @brief getter for the name of the id
     * @returns reference to a name of the id as const string
    */
    const std::string& getIdName() const noexcept;

    /** 
     * @brief initializes the name of the id
     * @param _idName - name of the id
    */
    void setIdName(const std::string& _idName);

    /** 
     * @brief getter for the value of the id
     * @returns reference to the value of the id as const string
    */
    const std::string& getValue() const noexcept;

    /** 
     * @brief initializes the value of the id
     * @param val - value of the id
    */
    void setValue(const std::string& val);

private:
    /// name of the id
    std::string idName;
    
    /// value of the id
    std::string value;
};

#endif