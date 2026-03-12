#include "../directive_analyzer.hpp"

#include <filesystem>
#include <format>
#include <utility>

#include "../../common/abstract-syntax-tree/ast_program.hpp"
#include "../../common/preprocessing/preprocessing_libraries.hpp"

DirectiveAnalyzer::DirectiveAnalyzer(std::unordered_map<std::string, std::vector<std::string>>& semErrors, const std::string& err)
    : semanticErrors{ semErrors }, globalError{ err } {}

void DirectiveAnalyzer::checkDir(const ASTProgram* program) {
    for(const auto& dir : program->getDirs()){
        switch(dir->getNodeType()){
            case ASTNodeType::INCLUDE:
                checkIncludeDir(static_cast<const ASTIncludeDir*>(dir.get()));
                break;
            default:
                std::unreachable();
        }
    }
}

void DirectiveAnalyzer::checkIncludeDir(const ASTIncludeDir* includeDir) {
    if(!std::filesystem::exists(Preprocessing::Libs::generateLibSourcePath(includeDir->getLibName()))){
        semanticErrors.at(globalError).push_back(
            std::format("Line {}, Column {}: SEMANTIC ERROR -> unknown library '{}'", 
                includeDir->getToken().line, includeDir->getToken().column, includeDir->getLibName())
        );
    }   
}