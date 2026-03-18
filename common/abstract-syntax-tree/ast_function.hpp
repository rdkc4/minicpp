#ifndef AST_FUNCTION_HPP
#define AST_FUNCTION_HPP

#include <memory>
#include <vector>

#include "ast_node.hpp"
#include "ast_parameter.hpp"
#include "ast_stmt.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ast_visitor.hpp"

/** 
 * @class ASTFunction
 * @brief AST node representing function node
*/
class ASTFunction final : public ASTNode {
public:
    /** 
     * @brief Creates the instance of the ast function
     * @param token - const reference to the token
     * @param type - return type of the function
    */
    ASTFunction(const Token& token, Type type = Type::NO_TYPE);

    /** 
     * @brief getter for the function parameters
     * @returns reference to a vector of pointers to parameters 
    */
    const std::vector<std::unique_ptr<ASTParameter>>& getParameters() const noexcept;

    /** 
     * @brief getter for the body of the function
     * @returns reference to a vector of pointers to statements of the body
    */
    const std::vector<std::unique_ptr<ASTStmt>>& getBody() const noexcept;

    /** 
     * @brief adds new parameter to parameters
     * @param parameter - parameter that is being added
    */
    void addParameter(std::unique_ptr<ASTParameter> parameter);

    /** 
     * @brief adds new statement to body
     * @param statement - pointer to a statement that is being added to body
    */
    void addStatement(std::unique_ptr<ASTStmt> statement);

    /** 
     * @brief sets predifined flag of a function
     * @param isPredefined - flag for predefined function
    */
    void setPredefined(bool isPredefined) noexcept;

    /** 
     * @brief getter for return type
     * @returns return type of the function
    */
    Type getType() const noexcept;

    /** 
     * @brief checks if function is predifined
     * @returns true if function is predefined, false otherwise
    */
    bool isPredefined() const noexcept;

    /** 
     * @brief getter for the amount of parameters
     * @returns number of parameters
    */
    size_t getParameterCount() const noexcept;

    /**
     * @brief sets the always return flag
     * @param returns - flag whether function returns or not
    */
    void setAlwaysReturns(bool returns) noexcept;

    /**
     * @brief getter for the always returns flag
     * @returns true if function always returns value, false otherwise
    */
    bool alwaysReturnsValue() const noexcept;

    /**
     * @brief accepts the ast visitor
     * @param visitor - reference to an ast visitor
    */
    void accept(ASTVisitor& visitor) override;

private:
    /// return type of the function
    Type type;

    /// flag if function is predefined
    bool predefined;

    /// flag if function always returns, only for functions that return value
    bool alwaysReturns;

    /// vector of pointers to parameters of the function
    std::vector<std::unique_ptr<ASTParameter>> parameters;

    /// vector of pointers to statements of the body
    std::vector<std::unique_ptr<ASTStmt>> body;

};

#endif