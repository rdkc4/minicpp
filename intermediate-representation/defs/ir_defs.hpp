#ifndef IR_DEFS_HPP
#define IR_DEFS_HPP

#include <cstddef>
#include <stack>
#include <string>
#include <vector>

struct IRThreadContext{
    // variable + temporary count
    size_t variableCount;
    // number to make temporary name unique
    size_t temporaries;
    // making sure nested temporary variables are handled well
    std::stack<std::string> temporaryNames;
    std::vector<std::string> errors;

    void init(){
        variableCount = 0;
        temporaries = 0;
    }

    size_t requiredMemory(size_t regSize){
        return variableCount * regSize;
    }

    void reset(){
        errors.clear();
    }

};

#endif