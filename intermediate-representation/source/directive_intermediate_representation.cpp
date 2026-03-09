#include "../directive_intermediate_representation.hpp"

#include <utility>

void DirectiveIntermediateRepresentation::directive(IRProgram* irProgram, const ASTProgram* astProgram){
    for(const auto& dir : astProgram->getDirectives()){
        switch(dir->getNodeType()){
            case ASTNodeType::INCLUDE:
                include(irProgram, static_cast<const ASTIncludeDir*>(dir.get()));
                break;
            default:
                std::unreachable();
        }
    }
}

void DirectiveIntermediateRepresentation::include(IRProgram* irProgram, const ASTIncludeDir* lib){
    irProgram->addLinkedLibrary(lib->getLibName());
}