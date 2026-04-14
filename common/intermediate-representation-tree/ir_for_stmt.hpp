#ifndef IR_FOR_STMT_HPP
#define IR_FOR_STMT_HPP

#include <memory>

#include "ir_stmt.hpp"
#include "ir_expr.hpp"
#include "ir_assign_stmt.hpp"
#include "../visitor/ir_visitor.hpp"

namespace IR::node {
    /**
     * @class IRForStmt
     * @brief IRT representation for the for-statement
    */
    class IRForStmt final : public IRStmt {
    public:
        /** 
         * @brief Creates the instance of the irt for-statement
        */
        IRForStmt();

        /**
         * @brief getter for the initializer of the for-statement node
         * @warning nullable
         * @returns pointer const pointer to the assignment statement
        */
        template<typename Self>
        decltype(auto) getInitializerStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).initializerStmt.get();
        }

        /**
         * @brief getter for the condition of the for-statement node
         * @warning nullable
         * @returns pointer const pointer to the condition of the for statement
        */
        template<typename Self>
        decltype(auto) getConditionExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).conditionExpr.get();
        }

        /**
         * @brief getter for the incrementer of the for-statement node
         * @warning nullable
         * @returns pointer or const pointer to the assignment statement
        */
        template<typename Self>
        decltype(auto) getIncrementerStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).incrementerStmt.get();
        }

        /**
         * @brief getter for the statement of the for-statement node
         * @returns pointer or const pointer to the statement
        */
        template<typename Self>
        decltype(auto) getStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).stmt.get();
        }

        /**
         * @brief initializes the for-statement node
         * @param initStmt - pointer to the assignment statement
         * @param condExpr - pointer to the condition of the for statement
         * @param incStmt - pointer to the assignment statement
         * @param statement - pointer to the statement
         * @param tempExpr - pointer to the temporaries, default nullptr
        */
        void setForSt(std::unique_ptr<IRAssignStmt> initStmt, std::unique_ptr<IRExpr> condExpr, std::unique_ptr<IRAssignStmt> incStmt, std::unique_ptr<IRStmt> statement, std::unique_ptr<IRTemporaryExpr> tempExpr = nullptr);

        /**
         * @brief checks if for-statement has initializes
         * @returns true when initializes is not nullptr, false otherwise
        */
        bool hasInitializerStmt() const noexcept;

        /**
         * @brief checks if for-statement has condition
         * @returns true if condition is not null, false otherwise
        */
        bool hasConditionExpr() const noexcept;

        /**
         * @brief checks if for-statement has incrementer
         * @returns true if the incrementer is not nullptr, false otherwise
        */
        bool hasIncrementerStmt() const noexcept;

        /**
         * @brief getter for the temporaries of the for-statement
         * @returns pointer or const pointer to the temporaries
        */
        template<typename Self>
        decltype(auto) getTemporaryExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).temporaryExpr.get();
        }

        /**
         * @brief checks if for-statement has temporaries
         * @returns true if temporaries are not nullptr, false otherwise
        */
        bool hasTemporaryExpr() const noexcept;

        /**
         * @brief accepts the ir visitor
         * @param visitor - reference to an ir visitor
        */
        void accept(IR::visitor::IRVisitor& visitor) override;

    private:
        /// pointer to the assignment statement of the initializer
        std::unique_ptr<IRAssignStmt> initializerStmt;

        /// pointer to the condition of the for-statement
        std::unique_ptr<IRExpr> conditionExpr;

        /// pointer to the assignment statement of the incrementer
        std::unique_ptr<IRAssignStmt> incrementerStmt;

        /// pointer to the statement of the for-statement
        std::unique_ptr<IRStmt> stmt;
        
        /// pointer to temporaries of the for-statement
        std::unique_ptr<IRTemporaryExpr> temporaryExpr;
    };

}

#endif
