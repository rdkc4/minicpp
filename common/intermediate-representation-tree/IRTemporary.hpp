#ifndef IRTEMPORARY_HPP
#define IRTEMPORARY_HPP

#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "IRNode.hpp"
#include "IRExpression.hpp"
#include "defs/IRTree_defs.hpp"
#include "../defs/defs.hpp"

/** 
 * @class IRTemporary
 * @brief IRT representation of the temporary variables
*/
class IRTemporary final : public IRNode {
public:
    /** 
     * @brief Creates the instance of the irt temporary
     * @param ntype - type of the irt node
    */
    IRTemporary(IRNodeType ntype);

    /** 
     * @brief getter for the temporaries
     * @returns reference to a const vector of pointers to expressions
    */
    const std::vector<std::unique_ptr<IRExpression>>& getTemporaries() const noexcept;

    /** 
     * @brief getter for the names of the temporaries
     * @returns reference to a const vector of strings that represent temporaries
    */
    const std::vector<std::string>& getTemporaryNames() const noexcept;

    /** 
     * @brief getter for the name of the temporary at the specified position
     * @param n - position of the temporary
     * @returns reference to a name of the temporary as const string
    */
    const std::string& getTemporaryNameAtN(size_t n) const noexcept;

    /** 
     * @brief getter for temporary at specified position
     * @param n - position of the temporary
     * @returns const pointer to the expression
    */
    const IRExpression* getExpressionAtN(size_t n) const noexcept;
    
    /** 
     * @brief getter for the temporary name and temporary expression at the specified position
     * @param n - position of the temporary
     * @returns pair of name as a string and pointer to the expression
    */
    std::pair<std::string, IRExpression*> getTemporaryNCAtN(size_t n) noexcept;

    /** 
     * @brief adds new temporary variable
     * @param tempName - name of the temporary variable
     * @param tempVal - pointer to the expression, default nullptr
     * @param type - type of the temporary variable, default NO_TYPE
     * @returns void
    */
    void addTemporary(const std::string& tempName, std::unique_ptr<IRExpression> tempVal = nullptr, Types type = Types::NO_TYPE);

    /** 
     * @brief initializes the temporary variable at specified position
     * @param tempVal - pointer to the expression
     * @param t - type of the temporary variable
     * @param n - position of the temporary
     * @returns void
    */
    void assignTempAtN(std::unique_ptr<IRExpression> tempVal, Types t, size_t n);

    /** 
     * @brief getter for the types of the temporaries
     * @returns reference to a const vector of types of the temporaries
    */
    const std::vector<Types>& getTypes() const noexcept;

    /** 
     * @brief prints formatted string of the temporary node
     * @note debugging purposes
     * @param offset - indentation
     * @returns void
    */
    void print(size_t offset) const override;

private:
    /// vector of types of the temporaries
    std::vector<Types> types;
    /// vector of names of the temporaries
    std::vector<std::string> tempNames;
    /// vector of pointers to expressions of the temporaries
    std::vector<std::unique_ptr<IRExpression>> temporaries;
};

#endif