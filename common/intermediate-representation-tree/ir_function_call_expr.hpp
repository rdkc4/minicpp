#ifndef IR_FUNCTION_CALL_EXPR_HPP
#define IR_FUNCTION_CALL_EXPR_HPP

#include <vector>
#include <string>
#include <memory>

#include "ir_expr.hpp"
#include "ir_temporary_expr.hpp"
#include "defs/ir_defs.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ir_visitor.hpp"

/** 
 * @class IRFunctionCallExpr
 * @brief IRT representation for the function call expression
*/
class IRFunctionCallExpr final : public IRExpr {
public:
    /** 
     * @brief Creates the instance of the irt function call expression
     * @param ntype - type of the irt node
     * @param callName - name of the function being called
     * @param type - return type of the function being called
    */
    IRFunctionCallExpr(IRNodeType ntype, const std::string& callName, Type type);

    /** 
     * @brief getter for the arguments of the function call expression
     * @returns reference to a const vector of pointers to arguments
    */
    const std::vector<std::unique_ptr<IRExpr>>& getArguments() const noexcept;

    /** 
     * @brief getter for the argument of the function call at specified position
     * @param n - position of the argument
     * @returns const pointer to the argument of the function call expression
    */
    const IRExpr* getArgumentAtN(size_t n) const noexcept;

    /** 
     * @brief getter for the temporaries of the function call expression
     * @returns reference to a const vector of pointers to the temporaries
    */
    const std::vector<std::unique_ptr<IRTemporaryExpr>>& getTemporaries() const noexcept;

    /** 
     * @brief adds new argument to the function call expression
     * @note each argument may have temporaries
     * @param argument - argument of the function call
     * @param temp - temporaries of the argument, default nullptr
    */
    void addArgument(std::unique_ptr<IRExpr> argument, std::unique_ptr<IRTemporaryExpr> temp = nullptr);

    /** 
     * @brief getter for the name of the function that is being called
     * @returns name of the called function
    */
    const std::string& getCallName() const noexcept;

    /** 
     * @brief initializes the name of the called function
     * @param _callName - name of the function that is being called
    */
    void setCallName(const std::string& _callName);

    /** 
     * @brief getter for the argument count
     * @returns number of arguments of the function call expression
    */
    size_t getArgumentCount() const noexcept;

    /**
     * @brief accepts the ir visitor
     * @param visitor - reference to an ir visitor
    */
    void accept(IRVisitor& visitor) override;

private:
    /// name of the function being called
    std::string callName;

    /// vector of pointers to expressions representing arguments
    std::vector<std::unique_ptr<IRExpr>> arguments;

    /// vector of pointers to temporaries of the function call expression
    std::vector<std::unique_ptr<IRTemporaryExpr>> temporaries;
    
};

#endif