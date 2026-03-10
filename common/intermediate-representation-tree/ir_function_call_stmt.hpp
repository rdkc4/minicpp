#ifndef IR_FUNCTION_CALL_STMT_HPP
#define IR_FUNCTION_CALL_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_function_call_expr.hpp"
#include "../visitor/ir_visitor.hpp"

/**
 * @class IRFunctionCallStmt
 * @brief IRT representation for the function call statement
*/
class IRFunctionCallStmt : public IRStmt {
public:
    /** 
     * @brief Creates the instance of the irt function-call statement
     * @param ntype - type of the irt node
    */
    IRFunctionCallStmt(IRNodeType ntype);

    /**
     * @brief getter for the function call expression of the function call statement
     * @returns pointer or const pointer to the function call expression
    */
    template<typename Self>
    decltype(auto) getFunctionCall(this Self&& self) noexcept {
        return std::forward<Self>(self).functionCall.get();
    }

    /**
     * @brief initializes the function call statement
     * @param call - pointer to a function call expression
    */
    void setFunctionCallSt(std::unique_ptr<IRFunctionCallExpr> call);

    /**
     * @brief accepts the ir visitor
     * @param visitor - reference to an ir visitor
    */
    void accept(IRVisitor& visitor) override;

private:
    /// pointer to the function call expression
    std::unique_ptr<IRFunctionCallExpr> functionCall; 
};

#endif