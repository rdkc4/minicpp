#ifndef AST_INCLUDE_DIR_HPP
#define AST_INCLUDE_DIR_HPP

#include <string>

#include "ast_dir.hpp"
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
    */
    ASTIncludeDir(const Token token);

    /** 
     * @brief getter for the name of the included library
     * @returns const reference to a lib name
    */
    const std::string& getLibName() const noexcept;

    /**
     * @brief initializes the name of the included library
     * @param includedLibName - name of the included library
    */
    void setLibName(const std::string& includedLibName);

    /**
     * @brief accepts the ast visitor
     * @param visitor - reference to an ast visitor
    */
    void accept(ASTVisitor& visitor) override;

private:
    /// name of the included library
    std::string libName;
};

#endif