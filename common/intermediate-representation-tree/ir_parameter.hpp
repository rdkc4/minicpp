#ifndef IR_PARAMETER_HPP
#define IR_PARAMETER_HPP

#include <string>
#include <string_view>

#include "ir_node.hpp"
#include "defs/ir_defs.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ir_visitor.hpp"

/** 
 * @class IRParameter
 * @brief IRT representation for the parameter node
*/
class IRParameter final : public IRNode {
public:
    /** 
     * @brief Creates the instance of the irt parameter
     * @param ntype - type of the irt node
     * @param parName - name of the parameter
     * @param type - type of the parameter
    */
    IRParameter(IRNodeType ntype, std::string_view parName, Type type);

    /** 
     * @brief getter for the name of the parameter
     * @returns reference to a parameter's name
    */
    const std::string& getParName() const noexcept;

    /** 
     * @brief initializes the parameter's name
     * @param _parName - the name of the parameter
    */
    void setParName(const std::string& _parName);

    /** 
     * @brief getter for the type of the parameter
     * @returns type of the parameter
    */
    Type getType() const noexcept;

    /** 
     * @brief initializes the type of the parameter
     * @param t - type of the parameter
    */
    void setType(Type t) noexcept;

    void accept(IRVisitor& visitor) override;

private:
    /// name of the parameter
    std::string parName;

    /// type of the parameter
    Type type;

};


#endif