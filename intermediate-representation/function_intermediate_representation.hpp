#ifndef FUNCTION_INTERMEDIATE_REPRESENTATION_HPP
#define FUNCTION_INTERMEDIATE_REPRESENTATION_HPP

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

#include "../common/abstract-syntax-tree/ast_function.hpp"
#include "../common/intermediate-representation-tree/ir_function.hpp"
#include "defs/ir_defs.hpp"
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
         * @param exceptions -reference to the map of functions and their exceptions
        */
        FunctionIntermediateRepresentation(std::unordered_map<std::string, std::vector<std::string>>& exceptions);

        /**
         * @brief getter for the context of the function processed by the current thread
         * @returns ir context of the current thread
        */
        static IR::defs::ctx::IRThreadContext& getContext() noexcept;

        /**
         * @brief turns ast function into irt function
         * @param astFunction - const pointer to the ast function
         * @returns pointer to the irt function
        */
        std::unique_ptr<node::IRFunction> transformFunction(const AST::node::ASTFunction* astFunction);

        /**
         * @brief turns ast parameters into irt parameters
         * @param irFunction - pointer to the irt function
         * @param astFunction - const pointer to the ast function
        */
        void transformParameters(IR::node::IRFunction* irFunction, const AST::node::ASTFunction* astFunction);

        /**
         * @brief turns body of the ast function into the body of the irt function
         * @param irFunction - pointer to the irt function
         * @param astFunction - const pointer to the ast function
        */
        void transformBody(IR::node::IRFunction* irFunction, const AST::node::ASTFunction* astFunction);

    private:
        /// thread local context of the function
        static thread_local IR::defs::ctx::IRThreadContext irContext;
        
        /// intermediate representation specialized for statements
        StatementIntermediateRepresentation stmtIR;

        /// maps function name to its exceptions
        std::unordered_map<std::string,std::vector<std::string>>& exceptions;

        /// mutex for concurrent access to exceptions
        std::mutex exceptionMtx;

    };

}

#endif