#ifndef ASTPROGRAM_HPP
#define ASTPROGRAM_HPP

#include <memory>

#include "ASTNode.hpp"
#include "ASTFunction.hpp"
#include "ASTDirective.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"

/** 
 * @class ASTProgram
 * @brief AST node representing the program 
*/
class ASTProgram final : public ASTNode {
public:
    /** 
     * @brief Creates the instance of the ast program
     * @param token - const reference to the token
     * @param ntype - type of the ast node
    */
    ASTProgram(const Token& token, ASTNodeType ntype);

    /** 
     * @brief getter for functions of the program
     * @returns reference to a vector of the pointers to functions
    */
    const std::vector<std::unique_ptr<ASTFunction>>& getFunctions() const noexcept;

    /** 
     * @brief getter for the amount of the functions
     * @returns number of the functions in a program
    */
    size_t getFunctionCount() const noexcept;

    /** 
     * @brief getter for a function at a specified index
     * @param n - index of the requested function
     * @returns const pointer to a function
    */
    const ASTFunction* getFunctionAtN(size_t n) const noexcept;

    /** 
     * @brief getter for directives of the program
     * @returns reference to a vector of the pointers to directives
    */
    const std::vector<std::unique_ptr<ASTDirective>>& getDirectives() const noexcept;

    /** 
     * @brief getter for a directive at a specified index
     * @param n - index of the requested directive
     * @returns const pointer to a directive
    */
    const ASTDirective* getDirectiveAtN(size_t n) const noexcept;

    /** 
     * @brief adds new function
     * @param function - pointer to a function that is being added
     * @returns void
    */
    void addFunction(std::unique_ptr<ASTFunction> function);

    /**
     * @brief adds new directive
     * @param directive - pointer to a directive that is being added
     * @returns void
    */
    void addDirective(std::unique_ptr<ASTDirective> directive);

    /** 
     * @brief prints program node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// vector of pointers to functions of the program
    std::vector<std::unique_ptr<ASTFunction>> functions;
    /// vector of pointers to directives of the program
    std::vector<std::unique_ptr<ASTDirective>> directives;

};

#endif