#ifndef DIRECTIVE_ANALYZER_HPP
#define DIRECTIVE_ANALYZER_HPP

#include <vector>
#include <memory>
#include <string>

#include "../common/abstract-syntax-tree/ASTDirective.hpp"
#include "../common/abstract-syntax-tree/ASTInclude.hpp"

class DirectiveAnalyzer {
public:
    void checkDirectives(std::vector<std::unique_ptr<ASTDirective>>& directives);

    void checkInclude(ASTInclude* lib);

private:
    std::vector<std::string> semanticErrors;
};

#endif