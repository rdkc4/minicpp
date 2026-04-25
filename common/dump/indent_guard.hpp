#ifndef INDENT_GUARD_HPP
#define INDENT_GUARD_HPP

/**
 * @namespace util
 * @brief module defining the utilities
*/
namespace util {
    /**
     * @namespace util::format
     * @brief module defining the utilities for formatting
    */
    namespace format {
        /**
         * @struct IndentGuard
         * @brief scope-based indentation control
        */
        struct IndentGuard {
        public:
            /**
             * @brief creates the instance of the indent guard and increments the indentation
             * @param indent - reference to indent
            */
            IndentGuard(int& indent) : indent{ indent } {
                ++indent;
            }

            /**
             * @brief deletes the indent guard instance and decrements the indentation
            */
            ~IndentGuard(){
                --indent;
            }

            /// deleted copy constructor
            IndentGuard(const IndentGuard&) = delete;

            /// deleted copy assignment operator 
            IndentGuard& operator=(const IndentGuard&) = delete;
            
            /// move constructor
            IndentGuard(IndentGuard&&) noexcept = delete;

            /// deleted move assignment operator
            IndentGuard& operator=(IndentGuard&&) noexcept = delete;

        private:
            /// reference to indent
            int& indent;

        };
    }
}

#endif