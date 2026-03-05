#ifndef DIRECTIVE_INTERMEDIATE_REPRESENTATION_HPP
#define DIRECTIVE_INTERMEDIATE_REPRESENTATION_HPP

#include "../common/abstract-syntax-tree/ASTProgram.hpp"
#include "../common/abstract-syntax-tree/ASTInclude.hpp"
#include "../common/intermediate-representation-tree/IRProgram.hpp"

class DirectiveIntermediateRepresentation {
public:
    void directive(IRProgram* irProgram, const ASTProgram* astProgram);
    
    void include(IRProgram* irProgram, const ASTInclude* lib);
};

#endif