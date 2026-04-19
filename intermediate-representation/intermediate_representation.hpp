#ifndef INTERMEDIATE_REPRESENTATION_HPP
#define INTERMEDIATE_REPRESENTATION_HPP

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

#include "../common/intermediate-representation-tree/ir_program.hpp"
#include "../common/abstract-syntax-tree/ast_program.hpp"
#include "../thread-pool/thread_pool.hpp"

/**
 * @namespace ir
 * @brief module defining the elements related to the intermediate representation
*/
namespace ir {
    /**
     * @class IntermediateRepresentation
     * @brief turns abstract syntax tree into intermediate representation tree
    */
    class IntermediateRepresentation {
    public:
        /** 
         * @brief Creates the instance of the intermediate representation
         * @param threadPool - reference to a thread pool
        */
        IntermediateRepresentation(util::concurrency::ThreadPool& threadPool);

        /**
         * @brief transforms ast program into irt program
         * @param program - const pointer to the root of the ast program
         * @returns pointer to the root of irt program
        */
        [[nodiscard]] std::unique_ptr<ir::IRProgram> 
        transformProgram(const syntax::ast::ASTProgram* program);

        /**
         * @brief checks if any errors are caught
         * @param program - const pointer to the root of the irt
         * @returns true if any error is caught, false otherwise
        */
        bool hasErrors(const ir::IRProgram* program) const noexcept;

        /**
         * @brief getter for the ir errors
         * @param program - pointer to the root of the irt program
         * @returns errors merged into a string
        */
        std::string getErrors(const ir::IRProgram* program) const noexcept;

    private:
        /// thread pool for parallel function ir transformation
        util::concurrency::ThreadPool& threadPool;

    protected:
        /// maps function name to its exceptions
        std::unordered_map<std::string,std::vector<std::string>> exceptions;

        /// mutex protecting exceptions map
        std::mutex mtx;

    };

}

#endif
