#ifndef DIRECTIVE_INTERMEDIATE_REPRESENTATION_HPP
#define DIRECTIVE_INTERMEDIATE_REPRESENTATION_HPP

#include "../common/abstract-syntax-tree/ast_program.hpp"
#include "../common/abstract-syntax-tree/ast_include_dir.hpp"
#include "../common/intermediate-representation-tree/ir_program.hpp"

namespace IR {
    /**
     * @class DirectiveIntermediateRepresentation
     * @brief turns ast directives into irt directives
    */
    class DirectiveIntermediateRepresentation {
    public:
        /**
         * @brief turns the ast directive into irt directive
         * @param irProgram - pointer to the irt program
         * @param astProgram - const pointer to the ast program
        */
        void transformDir(ir::IRProgram* irProgram, const syntax::ast::ASTProgram* astProgram);
        
        /**
         * @brief adds linked library to compile flags in irt program
         * @param irProgram - pointer to the irt program
         * @param astLib - pointer to the ast include directive
        */
        void transformIncludeDir(ir::IRProgram* irProgram, const syntax::ast::ASTIncludeDir* astLib);
    };

}

#endif