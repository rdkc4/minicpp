#ifndef ASM_FUNCTION_GENERATOR_HPP
#define ASM_FUNCTION_GENERATOR_HPP

#include <vector>
#include <string>

namespace AsmGenerator {

    /** 
     * @namespace Function
     * @brief Temporary module for predefined functions
    */
    namespace Function {

        /** 
         * @brief generates function that prints a number
         * @param printfFunc - reference to a vector for the asm code of the printf function
         * @returns void
        */
        void printfFunction(std::vector<std::string>& printfFunc);
    };
};

#endif
