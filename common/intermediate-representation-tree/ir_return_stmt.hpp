#ifndef IR_RETURN_STMT_HPP
#define IR_RETURN_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_temporary_expr.hpp"
#include "../visitor/ir_visitor.hpp"

namespace ir {
    /** 
     * @class IRReturnStmt
     * @brief IRT representation of the return statement
    */
    class IRReturnStmt final : public IRStmt {
    public:
        /** 
         * @brief Creates the instance of the irt return statement
        */
        IRReturnStmt();

        /** 
         * @brief getter for the expression of the return statement
         * @returns pointer or const pointer to the expression
        */
        template<typename Self>
        decltype(auto) getReturnExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).returnExpr.get();
        }

        /** 
         * @brief initializes the expression of the return statement
         * @param expr - pointer to the expression
         * @param tempExpr - pointer to the temporaries of the expression, default nullptr
        */
        void setReturnExpr(
            std::unique_ptr<IRExpr> expr, 
            std::unique_ptr<IRTemporaryExpr> tempExpr = nullptr
        );

        /** 
         * @brief getter for the temporaries
         * @returns pointer or const pointer to the temporary
        */
        template<typename Self>
        decltype(auto) getTemporaryExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).temporaryExpr.get();
        }

        /** 
         * @brief checks if return-statement returns value
         * @returns true if return-statement returns value, false otherwise
        */
        bool hasReturnValue() const noexcept;

        /** 
         * @brief checks if return statement has temporaries
         * @returns true if return statement has temporaries, false otherwise
        */
        bool hasTemporaryExpr() const noexcept;

        /**
         * @brief accepts the ir visitor
         * @param visitor - reference to an ir visitor
        */
        void accept(IRVisitor& visitor) override;

    private:
        /// pointer to the expression of the return statement
        std::unique_ptr<IRExpr> returnExpr;

        /// pointer to the temporaries of the return statement
        std::unique_ptr<IRTemporaryExpr> temporaryExpr;
        
    };

}

#endif