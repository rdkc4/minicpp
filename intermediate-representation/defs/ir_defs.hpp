#ifndef IR_DEFS_HPP
#define IR_DEFS_HPP

#include <cstddef>
#include <stack>
#include <string>
#include <vector>

/**
 * @struct IRThreadContext
 * @brief context of the thread performing the irt transformation
*/
struct IRThreadContext{
    /// number to make temporary name unique
    size_t temporaries;
    /// making sure nested temporary variables are handled properly
    std::stack<std::string> temporaryNames;
    /// vector of the errors of the function
    std::vector<std::string> errors;

    /** 
     * @brief initializes the context of the thread
    */
    void init(){
        temporaries = 0;
    }

    /** 
     * @brief resets the context of the thread
    */
    void reset(){
        errors.clear();
    }

};

#endif
