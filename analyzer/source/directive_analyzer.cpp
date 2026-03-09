#include "../directive_analyzer.hpp"

#include <filesystem>
#include <format>
#include <utility>

#include "../../common/preprocessing/preprocessing_libraries.hpp"

DirectiveAnalyzer::DirectiveAnalyzer(std::unordered_map<std::string, std::vector<std::string>>& semErrors, const std::string& err)
    : semanticErrors{ semErrors }, globalError{ err } {}

void DirectiveAnalyzer::checkDirectives(const std::vector<std::unique_ptr<ASTDir>>& directives) {
    for(auto& directive : directives){
        switch(directive->getNodeType()){
            case ASTNodeType::INCLUDE:
                checkInclude(static_cast<const ASTIncludeDir*>(directive.get()));
                break;
            default:
                std::unreachable();
        }
    }
}

void DirectiveAnalyzer::checkInclude(const ASTIncludeDir* _lib) {
    if(!std::filesystem::exists(Preprocessing::Libs::generateLibSourcePath(_lib->getLibName()))){
        semanticErrors.at(globalError).push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> unknown library '{}'", 
                _lib->getToken().line, _lib->getToken().column, _lib->getLibName())
        );
    }   
}