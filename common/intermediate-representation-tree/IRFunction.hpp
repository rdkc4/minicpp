#ifndef IRFUNCTION_HPP
#define IRFUNCTION_HPP

#include <memory>
#include <vector>
#include <string>

#include "IRNode.hpp"
#include "IRParameter.hpp"
#include "IRStatement.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

/**
 * @class IRFunction
 * @brief IRT representation for the function
*/
class IRFunction final : public IRNode {
public:
    IRFunction(IRNodeType ntype, const std::string& funcName, Types type);

    /** 
     * @brief getter for the parameters of the function
     * @returns reference to a const vector of pointers to parameters
    */
    const std::vector<std::unique_ptr<IRParameter>>& getParameters() const noexcept;

    /** 
     * @brief adds new parameter to the function
     * @param parameter - pointer to the parameter that is being added
     * @returns void
    */
    void addParameter(std::unique_ptr<IRParameter> parameter);

    /** 
     * @brief getter for the body of the function
     * @returns reference to a const vector of pointers to statements of the body
    */
    const std::vector<std::unique_ptr<IRStatement>>& getBody() const noexcept;

    /** 
     * @brief adds new statement to the body
     * @param statement - pointer to the statement that is being added
     * @returns void
    */
    void addStatement(std::unique_ptr<IRStatement> statement);

    /** 
     * @brief getter for the name of the function
     * @returns name of the function
    */
    const std::string& getFunctionName() const noexcept;

    /** 
     * @brief initializes function's name
     * @param funcName - name that is being assigned to the function
     * @returns void
    */
    void setFunctionName(const std::string& funcName);

    /** 
     * @brief getter for the return type of the function
     * @returns return type of the function
    */
    Types getType() const noexcept;

    /** 
     * @brief initializes the return type of the function
     * @param t - return type of the function
     * @returns void
    */
    void setType(Types t) noexcept;

    /** 
     * @brief getter for the amount of bytes function takes on the stack
     * @returns number of bytes that should be subtracted from the stack as string
    */
    const std::string& getRequiredMemory() const noexcept;

    /** 
     * @brief initializes the amount of bytes reserved for the stack
     * @param size - number of bytes reserved for the stack as string
     * @returns void
    */
    void setRequiredMemory(const std::string& size);

    /**
     * @brief eliminates statements of the function's body that appear after the node that always returns
     * @param startIdx - index in the vector of statements where deletion starts
     * @returns void
    */
    void eliminateDead(size_t startIdx);

    /** 
     * @brief prints formatted string of the function node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::string functionName;
    std::string requiredMemory;
    Types type;
    std::vector<std::unique_ptr<IRParameter>> parameters;
    std::vector<std::unique_ptr<IRStatement>> body;
};

#endif