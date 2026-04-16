#ifndef SCOPE_GUARD_HPP
#define SCOPE_GUARD_HPP

#include "scope_manager.hpp"

/**
 * @namespace Sym::guard
 * @brief module defining the safe mechanisms for symbol handling
*/
namespace Sym::guard {
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

    private:
        /// reference to a scope manager
        ScopeManager& scopeManager;

    };

}

#endif