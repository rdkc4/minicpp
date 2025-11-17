#ifndef IRPROGRAM_HPP
#define IRPROGRAM_HPP

#include <vector>
#include <memory>

#include "IRNode.hpp"
#include "IRFunction.hpp"
#include "defs/IRTree_defs.hpp"

/** 
 * @class IRProgram
 * @brief IRT representation of the program
*/
class IRProgram final : public IRNode {
public:
    IRProgram(IRNodeType ntype);

    /** 
     * @brief getter for the functions of the program
     * @returns reference to a const vector of pointers to the functions
    */
    const std::vector<std::unique_ptr<IRFunction>>& getFunctions() const noexcept;

    /** 
     * @brief getter for the function at the specified position
     * @param n - position of the function inside of a program
     * @returns const pointer to the function
    */
    const IRFunction* getFunctionAtN(size_t n) const noexcept;

    /** 
     * @brief allocates space for the functions in advance
     * @param n - number of functions
     * @returns void
    */
    void resizeFunctions(size_t n);

    /** 
     * @brief initializes the function at the specified position
     * @param _function - pointer to the function
     * @param n - position of the function
     * @returns void
    */
    void setFunctionAtN(std::unique_ptr<IRFunction> _function, size_t n);

    /** 
     * @brief getter for the count of the functions
     * @returns number of functions inside of the program
    */
    size_t getFunctionCount() const noexcept;

    /** 
     * @brief prints formatted string of the program node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    std::vector<std::unique_ptr<IRFunction>> functions;
};


#endif