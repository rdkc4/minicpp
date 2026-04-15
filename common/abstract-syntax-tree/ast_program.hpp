#ifndef AST_PROGRAM_HPP
#define AST_PROGRAM_HPP

#include <memory>

#include "ast_node.hpp"
#include "ast_function.hpp"
#include "ast_dir.hpp"
#include "../token/token.hpp"
#include "../visitor/ast_visitor.hpp"

namespace AST::node {
    /** 
     * @class ASTProgram
     * @brief AST node representing the program 
    */
    class ASTProgram final : public ASTNode {
    public:
        /** 
         * @brief Creates the instance of the ast program
         * @param token - const reference to the token
        */
        ASTProgram(const Token& token);

        /** 
         * @brief getter for functions of the program
         * @returns reference to a vector of the pointers to functions
        */
        const std::vector<std::unique_ptr<ASTFunction>>& getFunctions() const noexcept;

        /** 
         * @brief getter for the amount of the functions
         * @returns number of the functions in a program
        */
        size_t getFunctionCount() const noexcept;

        /** 
         * @brief getter for a function at a specified index
         * @param n - index of the requested function
         * @returns const pointer to a function
        */
        const ASTFunction* getFunctionAtN(size_t n) const noexcept;

        /** 
         * @brief getter for directives of the program
         * @returns reference to a vector of the pointers to directives
        */
        const std::vector<std::unique_ptr<ASTDir>>& getDirs() const noexcept;

        /** 
         * @brief getter for a directive at a specified index
         * @param n - index of the requested directive
         * @returns const pointer to a directive
        */
        const ASTDir* getDirAtN(size_t n) const noexcept;

        /** 
         * @brief adds new function
         * @param function - pointer to a function that is being added
        */
        void addFunction(std::unique_ptr<ASTFunction> function);

        /**
         * @brief adds new directive
         * @param directive - pointer to a directive that is being added
        */
        void addDir(std::unique_ptr<ASTDir> directive);

        /**
         * @brief accepts the ast visitor
         * @param visitor - reference to an ast visitor
        */
        void accept(AST::visitor::ASTVisitor& visitor) override;

    private:
        /// vector of pointers to functions of the program
        std::vector<std::unique_ptr<ASTFunction>> functions;
        
        /// vector of pointers to directives of the program
        std::vector<std::unique_ptr<ASTDir>> dirs;

    };

}

#endif