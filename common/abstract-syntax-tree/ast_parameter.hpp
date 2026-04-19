#ifndef AST_PARAMETER_HPP
#define AST_PARAMETER_HPP

#include "ast_node.hpp"
#include "../token/token.hpp"
#include "../defs/types.hpp"
#include "../visitor/ast_visitor.hpp"

namespace syntax::ast {
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
        ASTParameter(const syntax::Token& token, types::Type type = types::Type::NO_TYPE);
        
        /** 
         * @brief getter for a type of the parameter
         * @returns type of a parameter
        */
        types::Type getType() const noexcept;

        /** 
         * @brief initializes type of the parameter
         * @param parameterType - type of a parameter
        */
        void setType(types::Type parameterType) noexcept;

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(ASTVisitor& visitor) override;

    private:
        /// type of the parameter
        types::Type type;

    };

}

#endif