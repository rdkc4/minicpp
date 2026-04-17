#ifndef IR_WHILE_STMT_HPP
#define IR_WHILE_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_temporary_expr.hpp"
#include "../visitor/ir_visitor.hpp"

namespace ir {
    /** 
     * @class IRWhileStmt
     * @brief IRT representation of the while statement
    */
    class IRWhileStmt final : public IRStmt {
    public:
        /** 
         * @brief Creates the instance of the irt while-statement
        */
        IRWhileStmt();

        /** 
         * @brief getter for the condition of the while statement
         * @returns pointer or const pointer to condition of the while statement
        */
        template<typename Self>
        decltype(auto) getConditionExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).conditionExpr.get();
        }

        /** 
         * @brief getter for the statement of the while statement
         * @returns pointer or const pointer to the statement
        */
        template<typename Self>
        decltype(auto) getStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).stmt.get();
        }

        /** 
         * @brief initializes the while statement
         * @param condExpr - pointer to the condition of the while statement
         * @param statement - pointer to the statement
         * @param tempExpr - pointer to the temporary, default nullptr
        */
        void setWhileStmt(
            std::unique_ptr<IRExpr> condExpr, 
            std::unique_ptr<IRStmt> statement, 
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
         * @brief checks if the while statement has temporaries
         * @returns true if while statement has temporaries, false otherwise
        */
        bool hasTemporaryExpr() const noexcept;

        /**
         * @brief accepts the ir visitor
         * @param visitor - reference to an ir visitor
        */
        void accept(IRVisitor& visitor) override;

    private:
        /// pointer to the condition of the while-statement
        std::unique_ptr<IRExpr> conditionExpr;

        /// pointer to the statement of the while-statement
        std::unique_ptr<IRStmt> stmt;

        /// pointer to the temporaries of the while-statement
        std::unique_ptr<IRTemporaryExpr> temporaryExpr;
        
    };

}

#endif