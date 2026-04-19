#ifndef AST_VARIABLE_DECL_STMT_HPP
#define AST_VARIABLE_DECL_STMT_HPP

#include <memory>

#include "ast_stmt.hpp"
#include "ast_expr.hpp"
#include "../token/token.hpp"
#include "../defs/types.hpp"
#include "../visitor/ast_visitor.hpp"

namespace syntax::ast {
    /** 
     * @class ASTVariableDeclStmt
     * @brief AST node representing variable
    */
    class ASTVariableDeclStmt final : public ASTStmt {
    public:
        /** 
         * @brief Creates the instance of the ast variable declaration
         * @param token - const reference to the token
         * @param type - type of the variable
        */
        ASTVariableDeclStmt(const syntax::Token& token, types::Type type = types::Type::NO_TYPE);

        /** 
         * @brief getter for the type of the variable
         * @returns type of the variable
        */
        types::Type getType() const noexcept;

        /** 
         * @brief initializes the type of the variable
         * @param variableType - type of the variable
        */
        void setType(types::Type variableType) noexcept;

        /** 
         * @brief getter for the expression assigned to the variable
         * @warning nullable
         * @returns pointer or const pointer to the expression assigned to the variable
        */
        template<typename Self>
        decltype(auto) getAssignExpr(this Self&& self) noexcept {
            return std::forward<Self>(self).assignExpr.get();
        }

        /** 
         * @brief initializes expression of the variable
         * @param expr - expression being assigned to the variable
        */
        void setAssignExpr(std::unique_ptr<ASTExpr> expr);

        /** 
         * @brief checks if the variable has assignment
         * @return true if variable has assignment, false otherwise
        */
        bool hasAssignExpr() const noexcept;

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(ASTVisitor& visitor) override;

    private:
        /// type of the variable
        types::Type type;
        
        /// pointer to the expression assigned to the variable
        std::unique_ptr<ASTExpr> assignExpr;

    };

}

#endif