#include "../directive_intermediate_representation.hpp"

#include <utility>

void ir::DirectiveIntermediateRepresentation::transformDir(
    ir::IRProgram* irProgram, 
    const syntax::ast::ASTProgram* astProgram
){
    for(const auto& dir : astProgram->getDirs()){
        switch(dir->getNodeType()){
            case syntax::ast::ASTNodeType::INCLUDE_DIR:
                transformIncludeDir(irProgram, static_cast<const syntax::ast::ASTIncludeDir*>(dir.get()));
                break;
            default:
                std::unreachable();
        }
    }
}

void ir::DirectiveIntermediateRepresentation::transformIncludeDir(
    ir::IRProgram* irProgram, 
    const syntax::ast::ASTIncludeDir* astLib
){
    irProgram->addLinkedLib(astLib->getLibName());
}