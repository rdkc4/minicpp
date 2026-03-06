#ifndef DIRECTIVE_INTERMEDIATE_REPRESENTATION_HPP
#define DIRECTIVE_INTERMEDIATE_REPRESENTATION_HPP

#include "../common/abstract-syntax-tree/ASTProgram.hpp"
#include "../common/abstract-syntax-tree/ASTInclude.hpp"
#include "../common/intermediate-representation-tree/IRProgram.hpp"

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
    void directive(IRProgram* irProgram, const ASTProgram* astProgram);
    
    /**
     * @brief adds linked library to compile flags in irt program
     * @param irProgram - pointer to the irt program
     * @param lib - pointer to the data of the included library
    */
    void include(IRProgram* irProgram, const ASTInclude* lib);
};

#endif