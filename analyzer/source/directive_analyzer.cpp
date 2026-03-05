#include "../directive_analyzer.hpp"

#include <filesystem>
#include <format>
#include <utility>

#include "../../common/preprocessing/preprocessing_libraries.hpp"

void DirectiveAnalyzer::checkDirectives(std::vector<std::unique_ptr<ASTDirective>>& directives) {
    for(auto& directive : directives){
        switch(directive->getNodeType()){
            case ASTNodeType::INCLUDE:
                checkInclude(static_cast<ASTInclude*>(directive.get()));
                break;
            default:
                std::unreachable();
        }
    }
}

void DirectiveAnalyzer::checkInclude(ASTInclude* _lib) {
    if(!std::filesystem::exists(Preprocessing::Libs::generateLibSourcePath(_lib->getLibName()))){
        semanticErrors.push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> unknown library '{}'", 
                _lib->getToken().line, _lib->getToken().column, _lib->getLibName())
        );
    }   
}