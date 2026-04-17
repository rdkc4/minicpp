#ifndef AST_FOR_STMT_HPP
#define AST_FOR_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_assign_stmt.hpp"
#include "ast_expr.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

namespace syntax::ast {
    /** 
     * @class ASTForStmt
     * @brief AST node representing for statement
    */
    class ASTForStmt final : public ASTStmt {
    public:
        /** 
         * @brief Creates the instance of the ast for-statement
         * @param token - const reference to the token
        */
        ASTForStmt(const syntax::Token& token);

        /** 
         * @brief getter for initializer
         * @warning nullable
         * @returns pointer or const pointer to an assignment statement
        */
        template<typename Self>
        decltype(auto) getInitializerStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).initializerStmt.get();
        }

        /** 
         * @brief getter for condition 
         * @warning nullable
         * @returns pointer or const pointer to condition of the for statement
        */
        template<typename Self>
        decltype(auto) getConditionExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).conditionExpr.get();
        }

        /** 
         * @brief getter for incrementer
         * @warning nullable
         * @returns pointer or const pointer to assignment statement
        */
        template<typename Self>
        decltype(auto) getIncrementerStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).incrementerStmt.get();
        }
        
        /** 
         * @brief getter for statement
         * @returns pointer or const pointer to a statement
        */
        template<typename Self>
        decltype(auto) getStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).stmt.get();
        }

        /** 
         * @brief initializes for statement node
         * @param initStmt - pointer to an assignment statement (initializer) 
         * @param condExpr - pointer to a condition
         * @param incStmt - pointer to an assignment statement (incrementer)
         * @param statement - pointer to a statement node
        */
        void setForStmt(
            std::unique_ptr<ASTAssignStmt> initStmt, 
            std::unique_ptr<ASTExpr> condExpr, 
            std::unique_ptr<ASTAssignStmt> incStmt, 
            std::unique_ptr<ASTStmt> statement
        );

        /** 
         * @brief checks if initializer is initialized
         * @returns true if initializer is not null, otherwise false
        */
        bool hasInitializerStmt() const noexcept;

        /** 
         * @brief checks if condition is initialized
         * @returns true if condition is not null, otherwise false
        */
        bool hasConditionExpr() const noexcept;

        /** 
         * @brief checks if incrementer is initialized
         * @returns true if incrementer is not null, otherwise false
        */
        bool hasIncrementerStmt() const noexcept;

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(ASTVisitor& visitor) override;

    private:
        /// pointer to the assignment statement of the initializer
        std::unique_ptr<ASTAssignStmt> initializerStmt;

        /// pointer to the condition of the for-statement
        std::unique_ptr<ASTExpr> conditionExpr;

        /// pointer to the assignment statement of the incrementer
        std::unique_ptr<ASTAssignStmt> incrementerStmt;

        /// pointer to the statement of the for-statement
        std::unique_ptr<ASTStmt> stmt;

    };

}

#endif