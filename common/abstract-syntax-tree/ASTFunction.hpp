#ifndef ASTFUNCTION_HPP
#define ASTFUNCTION_HPP

#include <memory>
#include <vector>

#include "ASTNode.hpp"
#include "ASTParameter.hpp"
#include "ASTStatement.hpp"
#include "defs/ASTree_defs.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"

/** 
 * @class ASTFunction
 * @brief AST node representing function node
*/
class ASTFunction final : public ASTNode {
public:
    /** 
     * @brief Creates the instance of the ast function
     * @param token - const reference to the token
     * @param ntype - type of the ast node
     * @param type - return type of the function
    */
    ASTFunction(const Token& token, ASTNodeType ntype, Types type = Types::NO_TYPE);

    /** 
     * @brief getter for the function parameters
     * @returns reference to a vector of pointers to parameters 
    */
    const std::vector<std::unique_ptr<ASTParameter>>& getParameters() const noexcept;

    /** 
     * @brief getter for the body of the function
     * @returns reference to a vector of pointers to statements of the body
    */
    const std::vector<std::unique_ptr<ASTStatement>>& getBody() const noexcept;

    /** 
     * @brief adds new parameter to parameters
     * @param parameter - parameter that is being added
     * @returns void
    */
    void addParameter(std::unique_ptr<ASTParameter> parameter);

    /** 
     * @brief adds new statement to body
     * @param statement - pointer to a statement that is being added to body
     * @returns void
    */
    void addStatement(std::unique_ptr<ASTStatement> statement);

    /** 
     * @brief getter for return type
     * @returns return type of the function
    */
    Types getType() const noexcept;

    /** 
     * @brief getter for the amount of parameters
     * @returns number of parameters
    */
    size_t getParameterCount() const noexcept;

    /** 
     * @brief prints function node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// return type of the function
    Types type;
    std::vector<std::unique_ptr<ASTParameter>> parameters;
    std::vector<std::unique_ptr<ASTStatement>> body;

};

#endif