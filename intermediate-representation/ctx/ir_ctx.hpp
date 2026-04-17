#ifndef IR_CTX_HPP
#define IR_CTX_HPP

#include <cstddef>
#include <stack>
#include <string>
#include <vector>

namespace ir {
    /**
     * @struct IRFunctionContext
     * @brief context of the ir function
    */
    struct IRFunctionContext {
        /// number to make temporary name unique
        size_t temporaries{};

        /// making sure nested temporary variables are handled properly
        std::stack<std::string> temporaryNames;
        
        /// vector of the errors of the function
        std::vector<std::string> errors;

    };

}

#endif
