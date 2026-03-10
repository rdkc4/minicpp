#ifndef IR_PROGRAM_HPP
#define IR_PROGRAM_HPP

#include <vector>
#include <memory>
#include <string>
#include <unordered_set>

#include "ir_node.hpp"
#include "ir_function.hpp"
#include "defs/ir_defs.hpp"
#include "../visitor/ir_visitor.hpp"

/** 
 * @class IRProgram
 * @brief IRT representation of the program
*/
class IRProgram final : public IRNode {
public:
    /** 
     * @brief Creates the instance of the irt program
     * @param ntype - type of the irt node
    */
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
    */
    void resizeFunctions(size_t n);

    /** 
     * @brief initializes the function at the specified position
     * @param _function - pointer to the function
     * @param n - position of the function
    */
    void setFunctionAtN(std::unique_ptr<IRFunction> _function, size_t n);

    /** 
     * @brief getter for the count of the functions
     * @returns number of functions inside of the program
    */
    size_t getFunctionCount() const noexcept;

    /** 
     * @brief adds a new library to linking list
     * @param libName - name of the library
    */
    void addLinkedLibrary(const std::string& libName);

    /** 
     * @brief getter for the linked libraries
     * @returns linked libraries separated by ' '
    */
    std::string getLinkedLibs() const noexcept;
    
    /**
     * @brief accepts the ir visitor
     * @param visitor - reference to an ir visitor
    */
    void accept(IRVisitor& visitor) override;

private:
    /// vector of pointers to functions of the program
    std::vector<std::unique_ptr<IRFunction>> functions;

    /// linked libraries
    std::unordered_set<std::string> linkedLibs;
    
};


#endif