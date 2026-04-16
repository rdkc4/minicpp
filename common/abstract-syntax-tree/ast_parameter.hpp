#ifndef AST_PARAMETER_HPP
#define AST_PARAMETER_HPP

#include "ast_node.hpp"
#include "../token/token.hpp"
#include "../defs/defs.hpp"
#include "../visitor/ast_visitor.hpp"

namespace AST::node {
    /** 
     * @class ASTParameter
     * @brief AST node representing parameter
    */
    class ASTParameter final : public ASTNode {
    public:
        /** 
         * @brief Creates the instance of the ast parameter
         * @param token - const reference to the token
         * @param ntype - type of the ast node
         * @param type - type of the parameter
        */
        ASTParameter(const Token& token, Type type = Type::NO_TYPE);
        
        /** 
         * @brief getter for a type of the parameter
         * @returns type of a parameter
        */
        Type getType() const noexcept;

        /** 
         * @brief initializes type of the parameter
         * @param parameterType - type of a parameter
        */
        void setType(Type parameterType) noexcept;

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(AST::visitor::ASTVisitor& visitor) override;

    private:
        /// type of the parameter
        Type type;

    };

}

#endif