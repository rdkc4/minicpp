#ifndef FUNCTION_INTERMEDIATE_REPRESENTATION_HPP
#define FUNCTION_INTERMEDIATE_REPRESENTATION_HPP

#include <memory>

#include "../common/abstract-syntax-tree/ast_function.hpp"
#include "../common/intermediate-representation-tree/ir_function.hpp"
#include "ctx/ir_ctx.hpp"
#include "statement_intermediate_representation.hpp"

namespace IR {
    /**
     * @class FunctionIntermediateRepresentation
     * @brief turns ast function into irt function
    */
    class FunctionIntermediateRepresentation {
    public:
        /** 
         * @brief Creates the instance of the function intermediate representation
        */
        FunctionIntermediateRepresentation();

        /**
         * @brief turns ast function into irt function
         * @param astFunction - const pointer to the ast function
         * @returns pointer to the irt function
        */
        std::unique_ptr<ir::IRFunction> 
        transformFunction(const syntax::ast::ASTFunction* astFunction);

        /**
         * @brief turns ast parameters into irt parameters
         * @param irFunction - pointer to the irt function
         * @param astFunction - const pointer to the ast function
        */
        void transformParameters(
            ir::IRFunction* irFunction, 
            const syntax::ast::ASTFunction* astFunction
        );

        /**
         * @brief turns body of the ast function into the body of the irt function
         * @param irFunction - pointer to the irt function
         * @param astFunction - const pointer to the ast function
        */
        void transformBody(
            ir::IRFunction* irFunction, 
            const syntax::ast::ASTFunction* astFunction
        );

        /**
         * @brief getter for context of the function
         * @returns context of the function
        */
        const ir::IRFunctionContext& getContext() const noexcept;

    private:
        /// context of the function
        ir::IRFunctionContext ctx{};
        
        /// intermediate representation specialized for statements
        StatementIntermediateRepresentation stmtIR;

    };

}

#endif