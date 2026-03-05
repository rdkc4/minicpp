#ifndef ASTINCLUDE_HPP
#define ASTINCLUDE_HPP

#include "ASTDirective.hpp"
#include "defs/ASTree_defs.hpp"
#include <string>

/** 
 * @class ASTInclude
 * @brief AST node that represents include directive node
*/
class ASTInclude final : public ASTDirective {
public:
    /** 
     * @brief Creates the instance of the ast include directive
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTInclude(const Token token, ASTNodeType ntype);

    /** 
     * @brief getter for the name of the included library
     * @returns const reference to a lib name
    */
    const std::string& getLibName() const noexcept;

    /**
     * @brief initializes the name of the included library
     * @param _libName - name of the included library
     * @returns void 
    */
    void setLibName(const std::string _libName);

    /** 
     * @brief prints include directive node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// name of the included library
    std::string libName;
};

#endif