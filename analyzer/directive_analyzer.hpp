#ifndef DIRECTIVE_ANALYZER_HPP
#define DIRECTIVE_ANALYZER_HPP

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "../common/abstract-syntax-tree/ast_dir.hpp"
#include "../common/abstract-syntax-tree/ast_include_dir.hpp"

/**
 * @class DirectiveAnalyzer
 * @brief analyzer specialized for directive analysis
*/
class DirectiveAnalyzer {
public:
    /** 
     * @brief Creates the instance of the analyzer specialized for directives
     * @param scopeManager - reference to the global scope manager
     * @param semErrors - reference to the map connecting function names with their semantic errors
     * @param err - global error label
    */
    DirectiveAnalyzer(std::unordered_map<std::string, std::vector<std::string>>& semErrors, const std::string& err);

    /**
     * @brief checks the directives of the program
     * @param directives - const reference to a vector of pointers to ast directives
    */
    void checkDir(const std::vector<std::unique_ptr<ASTDir>>& directives);

    /**
     * @brief checks the included library
     * @param lib - const pointer to the included library
    */
    void checkIncludeDir(const ASTIncludeDir* lib);

private:
    /// maps functionName to its semantic errors
    std::unordered_map<std::string, std::vector<std::string>>& semanticErrors;

    /// identifier for global errors
    const std::string globalError;

};

#endif