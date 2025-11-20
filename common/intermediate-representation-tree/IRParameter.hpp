#ifndef IRPARAMETER_HPP
#define IRPARAMETER_HPP

#include <string>
#include <string_view>

#include "IRNode.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

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
    IRParameter(IRNodeType ntype, std::string_view parName, Types type);

    /** 
     * @brief getter for the name of the parameter
     * @returns reference to a parameter's name
    */
    const std::string& getParName() const noexcept;

    /** 
     * @brief initializes the parameter's name
     * @param _parName - the name of the parameter
     * @returns void
    */
    void setParName(const std::string& _parName);

    /** 
     * @brief getter for the type of the parameter
     * @returns type of the parameter
    */
    Types getType() const noexcept;

    /** 
     * @brief initializes the type of the parameter
     * @param t - type of the parameter
     * @returns void
    */
    void setType(Types t) noexcept;

    /** 
     * @brief prints formatted string of the parameter node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// name of the parameter
    std::string parName;
    /// type of the parameter
    Types type;
};


#endif