#ifndef SCOPE_GUARD_HPP
#define SCOPE_GUARD_HPP

#include "scope_manager.hpp"

namespace semantics {
    /**
     * @class ScopeGuard
     * @brief ensures proper scope handling
    */
    class ScopeGuard {
    public:
        /**
         * @brief creates the instance of the scope guard
         * @param scopeManager - reference to a scope manager
         * @details pushes new scope to scope manager
        */
        ScopeGuard(ScopeManager& scopeManager) : scopeManager{ scopeManager } {
            scopeManager.pushScope();
        }

        /**
         * @brief deletes the instance of the scope guard
         * @details pops the scope from the scope manager
        */
        ~ScopeGuard() {
            scopeManager.popScope();
        }

        /// deleted copy constructor
        ScopeGuard(const ScopeGuard&) = delete;

        /// deleted assignment operator
        ScopeGuard& operator=(const ScopeGuard&) = delete;

        /// move constructor
        ScopeGuard(ScopeGuard&&) = default;

        /// deleted move assignment operator
        ScopeGuard& operator=(ScopeGuard&&) = delete;

    private:
        /// reference to a scope manager
        ScopeManager& scopeManager;

    };

}

#endif