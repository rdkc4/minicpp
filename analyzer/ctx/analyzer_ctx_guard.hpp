#ifndef ANALYZER_CTX_GUARD_HPP
#define ANALYZER_CTX_GUARD_HPP

#include <string_view>

#include "analyzer_ctx.hpp"

namespace semantic {
    /**
     * @class AnalyzerContextGuard
     * @brief ensures proper context handling
    */
    class AnalyzerContextGuard {
    public:
        /**
         * @brief creates the instance of the analyzer context guard
         * @param ctx - reference to a thread context
         * @param functionName - name of the current function
         * @param scopeManager - pointer to a scope manager
         * @details initializes the context
        */
        AnalyzerContextGuard(
            AnalyzerThreadContext& ctx,
            std::string_view functionName,
            ScopeManager* scopeManager
        ) : ctx(ctx) {
            ctx.init(functionName, scopeManager);
        }

        /**
         * @brief deletes the instance of analyzer context guard
         * @details resets the context
        */
        ~AnalyzerContextGuard() {
            ctx.reset();
        }

        /// deleted copy constructor
        AnalyzerContextGuard(const AnalyzerContextGuard&) = delete;

        /// deleted assignment operator
        AnalyzerContextGuard& operator=(const AnalyzerContextGuard&) = delete;

        // deleted move constructor
        AnalyzerContextGuard(AnalyzerContextGuard&&) noexcept = delete;

        /// deleted move assignment operator
        AnalyzerContextGuard& operator=(AnalyzerContextGuard&&) noexcept = delete;

    private:
        /// reference to the analyzer thread context
        AnalyzerThreadContext& ctx;

    };

}

#endif