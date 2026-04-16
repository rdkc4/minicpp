#ifndef IR_FUNCTION_CALL_STMT_HPP
#define IR_FUNCTION_CALL_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_function_call_expr.hpp"
#include "../visitor/ir_visitor.hpp"

namespace IR::node {
    /**
     * @class IRFunctionCallStmt
     * @brief IRT representation for the function call statement
    */
    class IRFunctionCallStmt : public IRStmt {
    public:
        /** 
         * @brief Creates the instance of the irt function-call statement
        */
        IRFunctionCallStmt();

        /**
         * @brief getter for the function call expression of the function call statement
         * @returns pointer or const pointer to the function call expression
        */
        template<typename Self>
        decltype(auto) getFunctionCallExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).functionCallExpr.get();
        }

        /**
         * @brief initializes the function call statement
         * @param callExpr - pointer to a function call expression
        */
        void setFunctionCallStmt(std::unique_ptr<IRFunctionCallExpr> callExpr);

        /**
         * @brief accepts the ir visitor
         * @param visitor - reference to an ir visitor
        */
        void accept(IR::visitor::IRVisitor& visitor) override;

    private:
        /// pointer to the function call expression
        std::unique_ptr<IRFunctionCallExpr> functionCallExpr; 
    };

}

#endif