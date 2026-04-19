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

        private:
            /// reference to indent
            int& indent;

        };
    }
}

#endif