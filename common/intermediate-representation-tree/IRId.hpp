#ifndef IRID_HPP
#define IRID_HPP

#include <string>
#include <string_view>

#include "IRExpression.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

/**
 * @class IRId
 * @brief IRT representation for the id 
*/
class IRId final : public IRExpression {
public:
    /** 
     * @brief Creates the instance of the irt id
     * @param ntype - type of the irt node
     * @param idName - name of the id
     * @param type - type of the id
    */
    IRId(IRNodeType ntype, std::string_view idName, Types type);

    /** 
     * @brief getter for the name of the id
     * @returns reference to a name of the id as const string
    */
    const std::string& getIdName() const noexcept;

    /** 
     * @brief initializes the name of the id
     * @param _idName - name of the id
     * @returns void
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
     * @returns void
    */
    void setValue(const std::string& val);

    /** 
     * @brief prints formmatted string of the id node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// name of the id
    std::string idName;
    /// value of the id
    std::string value;
};

#endif