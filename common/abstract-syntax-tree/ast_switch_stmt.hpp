#ifndef AST_SWITCH_STMT_HPP
#define AST_SWITCH_STMT_HPP

#include <memory>
#include <vector>

#include "ast_stmt.hpp"
#include "ast_id_expr.hpp"
#include "ast_case_stmt.hpp"
#include "ast_default_stmt.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

namespace AST::node {
    /** 
     * @class ASTSwitchStmt
     * @brief AST node representing switch statement
    */
    class ASTSwitchStmt final : public ASTStmt {
    public:
        /** 
         * @brief Creates the instance of the ast switch-statement
         * @param token - const reference to the token
        */
        ASTSwitchStmt(const Token& token);

        /** 
         * @brief getter for a variable (id of a variable)
         * @returns pointer or const pointer to id
        */
        template<typename Self>
        decltype(auto) getVariableIdExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).variableIdExpr.get();
        }

        /** 
         * @brief initializes id of a variable
         * @param idExpr - pointer to id of a variable
        */
        void setVariableIdExpr(std::unique_ptr<ASTIdExpr> idExpr);

        /** 
         * @brief getter for the cases of the switch statement
         * @returns reference to a vector of pointers to cases
        */
        const std::vector<std::unique_ptr<ASTCaseStmt>>& getCaseStmts() const noexcept;

        /** 
         * @brief getter for the default statement of the switch statement
         * @returns pointer or const pointer to a default statement
        */
        template<typename Self>
        decltype(auto) getDefaultStmt(this Self&& self) noexcept {
            return std::forward<Self>(self).defaultStmt.get();
        }

        /** 
         * @brief adds new case
         * @param caseStmt - pointer to a case that is being added
        */
        void addCaseStmt(std::unique_ptr<ASTCaseStmt> caseStmt);

        /** 
         * @brief adds default statement
         * @param swDefaultStmt - pointer to a default statement
        */
        void setDefaultStmt(std::unique_ptr<ASTDefaultStmt> swDefaultStmt);

        /** 
         * @brief checks if default statement exists
         * @returns true if default exists, false otherwise
        */
        bool hasDefaultStmt() const noexcept;

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(AST::visitor::ASTVisitor& visitor) override;

    private:
        /// pointer to id of the variable of the switch-statement
        std::unique_ptr<ASTIdExpr> variableIdExpr;

        /// vector of pointer to cases of the switch-statement
        std::vector<std::unique_ptr<ASTCaseStmt>> caseStmts;
        
        /// pointer to the default case of the switch-statement
        std::unique_ptr<ASTDefaultStmt> defaultStmt;

    };

}

#endif