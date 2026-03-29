#ifndef ANALYZER_TEST_HPP
#define ANALYZER_TEST_HPP

#include <string>
#include <vector>
#include <cassert>

#include "../../analyzer/analyzer.hpp"

class AnalyzerTest : public Analyzer {
    public:
        AnalyzerTest(ScopeManager& scopeManager, ThreadPool& threadPool) : Analyzer{ scopeManager, threadPool }{}

        ScopeManager& getScopeManager() noexcept {
            return globalScopeManager;
        }

        const char* getGlobalErrLabel() const noexcept {
            return globalError;
        }

        const std::vector<std::string>& getErrors(const std::string& func) const noexcept {
            assert(semanticErrors.find(func) != semanticErrors.end());
            return semanticErrors.at(func);
        }

        AnalyzerThreadContext& getContext() noexcept {
            return analyzerContext;
        }
};

#endif