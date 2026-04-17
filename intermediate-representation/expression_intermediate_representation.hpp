#ifndef EXPRESSION_INTERMEDIATE_REPRESENTATION_HPP
#define EXPRESSION_INTERMEDIATE_REPRESENTATION_HPP

#include <memory>

#include "../common/abstract-syntax-tree/ast_expr.hpp"
#include "../common/abstract-syntax-tree/ast_binary_expr.hpp"
#include "../common/abstract-syntax-tree/ast_id_expr.hpp"
#include "../common/abstract-syntax-tree/ast_literal_expr.hpp"
#include "../common/abstract-syntax-tree/ast_function_call_expr.hpp"

#include "../common/intermediate-representation-tree/ir_expr.hpp"
#include "../common/intermediate-representation-tree/ir_literal_expr.hpp"
#include "../common/intermediate-representation-tree/ir_id_expr.hpp"
#include "../common/intermediate-representation-tree/ir_function_call_expr.hpp"
#include "ctx/ir_ctx.hpp"

namespace IR {
    /**
     * @class ExpressionIntermediateRepresentation
     * @brief turns ast expression into irt expression
    */
    class ExpressionIntermediateRepresentation {
    public:
        /** 
         * @brief Creates the instance of the expression intermediate representation
        */
        ExpressionIntermediateRepresentation(ir::IRFunctionContext& context);

        /**
         * @brief turns ast expression into irt expression
         * @param astExpr - const pointer to the ast expression
         * @returns pointer to the irt expression
        */
        std::unique_ptr<ir::IRExpr> transformExpr(const syntax::ast::ASTExpr* astExpr);

        /**
         * @brief turns ast binary expression into irt binary expression
         * @param astBinaryExpr - const pointer to the ast binary expression
         * @returns pointer to the irt expression
        */
        std::unique_ptr<ir::IRExpr> 
        transformBinaryExpr(const syntax::ast::ASTBinaryExpr* astBinaryExpr);

        /**
         * @brief turns ast id into irt id
         * @param astIdExpr - const pointer to the ast id
         * @returns pointer to the irt id
        */
        std::unique_ptr<ir::IRIdExpr> 
        transformIdExpr(const syntax::ast::ASTIdExpr* astIdExpr) const;

        /**
         * @brief turns ast literal into irt literal
         * @param astLiteralExpr - const pointer to the ast literal
         * @returns pointer to the irt literal
        */
        std::unique_ptr<ir::IRLiteralExpr> 
        transformLiteralExpr(const syntax::ast::ASTLiteralExpr* astLiteralExpr) const;

        /**
         * @brief generates temporary variables for runction calls of the expression
         * @param astExpr - const pointer to the ast expression
         * @returns pointer to the temporary variables
        */
        std::unique_ptr<ir::IRTemporaryExpr> 
        initiateTemporaries(const syntax::ast::ASTExpr* astExpr);

        /**
         * @brief turns ast function call into irt function call
         * @param astCallExpr - const pointer to the ast function call
         * @returns pointer to the irt function call
        */
        std::unique_ptr<ir::IRFunctionCallExpr> 
        transformFunctionCallExpr(const syntax::ast::ASTFunctionCallExpr* astCallExpr);

        /**
         * @brief turns arguments of the ast function call into arguments of the irt function call
         * @param irCallExpr - pointer to the irt function call
         * @param astCallExpr - const pointer to the ast function call
        */
        void transformArguments(
            ir::IRFunctionCallExpr* irCallExpr, 
            const syntax::ast::ASTFunctionCallExpr* astCallExpr
        );

        /**
         * @brief counting the number of required temporaries
         * @param astExpr - const pointer to the ast expression
         * @returns number of required temporaries
        */
        size_t countTemporaries(const syntax::ast::ASTExpr* astExpr) const;

        /**
         * @brief generates the name for the temporary variable
         * @returns name of the temporary variable
        */
        std::string generateTemporaries();

        /**
         * @brief assigns function calls to the temporaries
         * @param temporaryRoot - pointer to the parent node of the temporaries
         * @param astExpr - const pointer to the ast expression
         * @param idx - index of the temporary being assigned
        */
        void assignTemporaries(
            ir::IRTemporaryExpr* temporaryRoot, 
            const syntax::ast::ASTExpr* astExpr, 
            size_t& idx
        );

        /**
         * @brief replaces function calls with temporaries
         * @param astCallExpr - const pointer to the ast function call
         * @returns pointer to the irt id of the temporary variable
        */
        std::unique_ptr<ir::IRIdExpr> 
        replaceFunctionCallExpr(const syntax::ast::ASTFunctionCallExpr* astCallExpr);

    private:
        /// reference to a function context
        ir::IRFunctionContext& ctx;

    };

}

#endif