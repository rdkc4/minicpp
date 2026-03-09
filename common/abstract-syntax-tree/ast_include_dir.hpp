#ifndef AST_INCLUDE_DIR_HPP
#define AST_INCLUDE_DIR_HPP

#include <string>

#include "ast_dir.hpp"
#include "defs/ast_defs.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTIncludeDir
 * @brief AST node that represents include directive node
*/
class ASTIncludeDir final : public ASTDir {
public:
    /** 
     * @brief Creates the instance of the ast include directive
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTIncludeDir(const Token token, ASTNodeType ntype);

    /** 
     * @brief getter for the name of the included library
     * @returns const reference to a lib name
    */
    const std::string& getLibName() const noexcept;

    /**
     * @brief initializes the name of the included library
     * @param _libName - name of the included library
    */
    void setLibName(const std::string _libName);

    void accept(ASTVisitor& visitor) override;

private:
    /// name of the included library
    std::string libName;
};

#endif