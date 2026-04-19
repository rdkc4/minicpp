#ifndef FUNCTION_CODE_GENERATOR_HPP
#define FUNCTION_CODE_GENERATOR_HPP

#include "../../common/intermediate-representation-tree/ir_function.hpp"
#include "statement_code_generator.hpp"
#include "ctx/code_generator_ctx.hpp"

namespace code_gen {
    /** 
     * @class FunctionCodeGenerator
     * @brief code generator specialized for the functions
    */
    class FunctionCodeGenerator {
    public:
        /** 
         * @brief Creates the instance of the function code generator
         * @param asmCode - reference to a map function_name:code_of_the_function
        */
        FunctionCodeGenerator();

        /** 
         * @brief generates the asm code of the function
         * @param function - const pointer to the irt function
        */
        void generateFunction(const ir::IRFunction* function);

        /** 
         * @brief generates the parameters of the function
         * @param function - const pointer to the irt function
        */
        void generateParameters(const ir::IRFunction* function);

        /**
         * @brief getter for the context of the function
         * @returns const reference to a context of the function
        */
        const CodeGeneratorFunctionContext& getContext() const noexcept;

    private:
        /// context of the function
        CodeGeneratorFunctionContext ctx{};

        /// code generator specialized for statements
        StatementCodeGenerator stmtGenerator;

    };

}

#endif