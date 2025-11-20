#ifndef IRFUNCTION_CALL_HPP
#define IRFUNCTION_CALL_HPP

#include <vector>
#include <string>
#include <memory>

#include "IRExpression.hpp"
#include "IRTemporary.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

/** 
 * @class IRFunctionCall
 * @brief IRT representation for the function call
*/
class IRFunctionCall final : public IRExpression {
public:
    /** 
     * @brief Creates the instance of the irt function call
     * @param ntype - type of the irt node
     * @param callName - name of the function being called
     * @param type - return type of the function being called
    */
    IRFunctionCall(IRNodeType ntype, const std::string& callName, Types type);

    /** 
     * @brief getter for the arguments of the function call
     * @returns reference to a const vector of pointers to arguments
    */
    const std::vector<std::unique_ptr<IRExpression>>& getArguments() const noexcept;

    /** 
     * @brief getter for the argument of the function call at specified position
     * @param n - position of the argument
     * @returns const pointer to the argument of the function call
    */
    const IRExpression* getArgumentAtN(size_t n) const noexcept;

    /** 
     * @brief getter for the temporaries of the function call
     * @returns reference to a const vector of pointers to the temporaries
    */
    const std::vector<std::unique_ptr<IRTemporary>>& getTemporaries() const noexcept;

    /** 
     * @brief adds new argument to the function call
     * @note each argument may have temporaries
     * @param argument - argument of the function call
     * @param temp - temporaries of the argument, default nullptr
     * @returns void
    */
    void addArgument(std::unique_ptr<IRExpression> argument, std::unique_ptr<IRTemporary> temp = nullptr);

    /** 
     * @brief getter for the name of the function that is being called
     * @returns name of the called function
    */
    const std::string& getCallName() const noexcept;

    /** 
     * @brief initializes the name of the called function
     * @param _callName - name of the function that is being called
     * @returns void
    */
    void setCallName(const std::string& _callName);

    /** 
     * @brief getter for the argument count
     * @returns number of arguments of the function call
    */
    size_t getArgumentCount() const noexcept;

    /** 
     * @brief prints formatted string of the function call node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// name of the function being called
    std::string callName;
    /// vector of pointers to expressions representing arguments
    std::vector<std::unique_ptr<IRExpression>> arguments;
    /// vector of pointers to temporaries of the function call
    std::vector<std::unique_ptr<IRTemporary>> temporaries;
};

#endif