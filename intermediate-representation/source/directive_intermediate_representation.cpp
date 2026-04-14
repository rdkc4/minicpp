#include "../directive_intermediate_representation.hpp"

#include <utility>

void IR::DirectiveIntermediateRepresentation::transformDir(IR::node::IRProgram* irProgram, const ASTProgram* astProgram){
    for(const auto& dir : astProgram->getDirs()){
        switch(dir->getNodeType()){
            case ASTNodeType::INCLUDE_DIR:
                transformIncludeDir(irProgram, static_cast<const ASTIncludeDir*>(dir.get()));
                break;
            default:
                std::unreachable();
        }
    }
}

void IR::DirectiveIntermediateRepresentation::transformIncludeDir(IR::node::IRProgram* irProgram, const ASTIncludeDir* astLib){
    irProgram->addLinkedLib(astLib->getLibName());
}