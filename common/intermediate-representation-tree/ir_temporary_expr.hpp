#ifndef IR_TEMPORARY_EXPR_HPP
#define IR_TEMPORARY_EXPR_HPP

#include <memory>
#include <vector>
#include <string>
#include <utility>

#include "ir_node.hpp"
#include "ir_expr.hpp"
#include "defs/ir_defs.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ir_visitor.hpp"

/** 
 * @class IRTemporaryExpr
 * @brief IRT representation of the temporary variables
*/
class IRTemporaryExpr final : public IRNode {
public:
    /** 
     * @brief Creates the instance of the irt temporary
     * @param ntype - type of the irt node
    */
    IRTemporaryExpr(IRNodeType ntype);

    /** 
     * @brief getter for the temporaries
     * @returns reference to a const vector of pointers to expressions
    */
    const std::vector<std::unique_ptr<IRExpr>>& getTemporaries() const noexcept;

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
    const IRExpr* getExpressionAtN(size_t n) const noexcept;
    
    /** 
     * @brief getter for the temporary name and temporary expression at the specified position
     * @param n - position of the temporary
     * @returns pair of name as a string and pointer to the expression
    */
    std::pair<std::string, IRExpr*> getTemporaryNCAtN(size_t n) noexcept;

    /** 
     * @brief adds new temporary variable
     * @param tempName - name of the temporary variable
     * @param tempVal - pointer to the expression, default nullptr
     * @param type - type of the temporary variable, default NO_TYPE
    */
    void addTemporary(const std::string& tempName, std::unique_ptr<IRExpr> tempVal = nullptr, Type type = Type::NO_TYPE);

    /** 
     * @brief initializes the temporary variable at specified position
     * @param tempVal - pointer to the expression
     * @param t - type of the temporary variable
     * @param n - position of the temporary
    */
    void assignTempAtN(std::unique_ptr<IRExpr> tempVal, Type t, size_t n);

    /** 
     * @brief getter for the types of the temporaries
     * @returns reference to a const vector of types of the temporaries
    */
    const std::vector<Type>& getTypes() const noexcept;

    /**
     * @brief accepts the ir visitor
     * @param visitor - reference to an ir visitor
    */
    void accept(IRVisitor& visitor) override;

private:
    /// vector of types of the temporaries
    std::vector<Type> types;
    
    /// vector of names of the temporaries
    std::vector<std::string> tempNames;

    /// vector of pointers to expressions of the temporaries
    std::vector<std::unique_ptr<IRExpr>> temporaries;
    
};

#endif