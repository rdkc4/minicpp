#ifndef IR_DEFS_HPP
#define IR_DEFS_HPP

#include <cstddef>
#include <stack>
#include <string>

struct IRThreadContext{
    // current function's variable count
    size_t variableCount;
    // count of the temporary variables for function calls
    size_t temporaries;
    // making sure nested temporary variables are handled well
    std::stack<std::string> temporaryNames;

    void init(){
        variableCount = 0;
        temporaries = 0;
    }

    size_t requiredMemory(size_t regSize){
        return (variableCount + temporaries) * regSize;
    }

    // no need for reset method, varCount and tmpCount are reseting at init and tmpNames will be empty
};

#endif